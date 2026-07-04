#include "pch.h"
#include "HutaoNativeLoopbackSupport.h"
#include "HutaoString.h"
#include "Array.h"
#include <sddl.h>

HutaoNativeLoopbackSupport::HutaoNativeLoopbackSupport()
    : m_firewallApiModule(nullptr)
    , m_firewallPolicy(nullptr)
    , m_pEnumAppContainers(nullptr)
    , m_pFreeAppContainers(nullptr)
    , m_pGetAppContainerConfig(nullptr)
    , m_pSetAppContainerConfig(nullptr)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        return;
    }
}

HutaoNativeLoopbackSupport::~HutaoNativeLoopbackSupport()
{
    if (m_firewallPolicy)
    {
        m_firewallPolicy->Release();
        m_firewallPolicy = nullptr;
    }
    if (m_firewallApiModule)
    {
        FreeLibrary(m_firewallApiModule);
        m_firewallApiModule = nullptr;
    }
    CoUninitialize();
}

HRESULT HutaoNativeLoopbackSupport::LoadNetworkIsolationApi()
{
    if (m_pEnumAppContainers)
    {
        return S_OK;
    }

    m_firewallApiModule = LoadLibraryW(L"FirewallAPI.dll");
    if (!m_firewallApiModule)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        ThrowForHR(hr, "Failed to load FirewallAPI.dll");
        return hr;
    }

    m_pEnumAppContainers = reinterpret_cast<FN_EnumAppContainers>(
        GetProcAddress(m_firewallApiModule, "NetworkIsolationEnumAppContainers"));
    m_pFreeAppContainers = reinterpret_cast<FN_FreeAppContainers>(
        GetProcAddress(m_firewallApiModule, "NetworkIsolationFreeAppContainers"));
    m_pGetAppContainerConfig = reinterpret_cast<FN_GetConfig>(
        GetProcAddress(m_firewallApiModule, "NetworkIsolationGetAppContainerConfig"));
    m_pSetAppContainerConfig = reinterpret_cast<FN_SetConfig>(
        GetProcAddress(m_firewallApiModule, "NetworkIsolationSetAppContainerConfig"));

    if (!m_pEnumAppContainers || !m_pFreeAppContainers || !m_pGetAppContainerConfig || !m_pSetAppContainerConfig)
    {
        HRESULT hr = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
        ThrowForHR(hr, "Failed to get Network Isolation API function pointers");
        return hr;
    }

    return S_OK;
}

HRESULT HutaoNativeLoopbackSupport::FindAppContainerSid(PCWSTR familyName, PSID* outSid)
{
    AssertNonNullAndReturn(outSid);
    *outSid = nullptr;
    AssertNonNullAndReturn(familyName);

    HRESULT hr = LoadNetworkIsolationApi();
    if (FAILED(hr))
    {
        return hr;
    }

    DWORD count = 0;
    PINET_FIREWALL_APP_CONTAINER containers = nullptr;
    DWORD err = m_pEnumAppContainers(0, &count, &containers);
    if (err != ERROR_SUCCESS)
    {
        hr = HRESULT_FROM_WIN32(err);
        ThrowForHR(hr, "NetworkIsolationEnumAppContainers failed");
        if (containers)
        {
            m_pFreeAppContainers(containers);
        }
        return hr;
    }

    PSID foundSid = nullptr;
    for (DWORD i = 0; i < count; ++i)
    {
        if (containers[i].appContainerName &&
            _wcsicmp(containers[i].appContainerName, familyName) == 0)
        {
            DWORD sidSize = GetLengthSid(containers[i].appContainerSid);
            foundSid = static_cast<PSID>(malloc(sidSize));
            if (foundSid)
            {
                if (!CopySid(sidSize, foundSid, containers[i].appContainerSid))
                {
                    free(foundSid);
                    foundSid = nullptr;
                }
            }
            break;
        }
    }

    m_pFreeAppContainers(containers);

    if (!foundSid)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        ThrowForHR(hr, "App Container not found for the specified package family name");
        return hr;
    }

    *outSid = foundSid;
    return S_OK;
}

HRESULT HutaoNativeLoopbackSupport::CreateHutaoStringFromSid(PSID sid, IHutaoString** outString)
{
    AssertNonNullAndReturn(outString);
    *outString = nullptr;
    AssertNonNullAndReturn(sid);

    LPWSTR sidString = nullptr;
    if (!ConvertSidToStringSidW(sid, &sidString))
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        ThrowForHR(hr, "ConvertSidToStringSidW failed");
        return hr;
    }

    HutaoString* str = new HutaoString(sidString);
    LocalFree(sidString);

    *outString = static_cast<IHutaoString*>(str);
    return S_OK;
}

HRESULT __stdcall HutaoNativeLoopbackSupport::IsEnabled(PCWSTR familyName, IHutaoString** sid, boolean* enabled)
{
    AssertNonNullAndReturn(enabled);
    AssertNonNullAndReturn(familyName);
    AssertNonNullAndReturn(sid);

    *enabled = false;
    *sid = nullptr;

    HRESULT hr = LoadNetworkIsolationApi();
    if (FAILED(hr))
    {
        return hr;
    }

    // Find the target app container SID
    PSID targetSid = nullptr;
    hr = FindAppContainerSid(familyName, &targetSid);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create the SID string output
    hr = CreateHutaoStringFromSid(targetSid, sid);
    if (FAILED(hr))
    {
        free(targetSid);
        return hr;
    }

    // Get the current exemption list
    DWORD curCount = 0;
    PSID_AND_ATTRIBUTES curList = nullptr;
    DWORD err = m_pGetAppContainerConfig(&curCount, &curList);
    if (err != ERROR_SUCCESS)
    {
        free(targetSid);
        hr = HRESULT_FROM_WIN32(err);
        ThrowForHR(hr, "NetworkIsolationGetAppContainerConfig failed in IsEnabled");
        return hr;
    }

    // Check if target SID is in the exemption list
    *enabled = false;
    for (DWORD i = 0; i < curCount; ++i)
    {
        if (EqualSid(targetSid, curList[i].Sid))
        {
            *enabled = true;
            break;
        }
    }

    // Cleanup GetConfig memory (allocated on process heap per MSDN)
    if (curList)
    {
        HANDLE heap = GetProcessHeap();
        for (DWORD i = 0; i < curCount; ++i)
        {
            if (curList[i].Sid)
            {
                HeapFree(heap, 0, curList[i].Sid);
            }
        }
        HeapFree(heap, 0, curList);
    }

    free(targetSid);
    return S_OK;
}

HRESULT __stdcall HutaoNativeLoopbackSupport::Enable(PCWSTR familyName, IHutaoString** sid)
{
    AssertNonNullAndReturn(familyName);
    AssertNonNullAndReturn(sid);

    *sid = nullptr;

    HRESULT hr = LoadNetworkIsolationApi();
    if (FAILED(hr))
    {
        return hr;
    }

    // Find the target app container SID
    PSID targetSid = nullptr;
    hr = FindAppContainerSid(familyName, &targetSid);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create the SID string output
    hr = CreateHutaoStringFromSid(targetSid, sid);
    if (FAILED(hr))
    {
        free(targetSid);
        return hr;
    }

    // Get the current exemption list
    DWORD curCount = 0;
    PSID_AND_ATTRIBUTES curList = nullptr;
    DWORD err = m_pGetAppContainerConfig(&curCount, &curList);
    if (err != ERROR_SUCCESS)
    {
        free(targetSid);
        hr = HRESULT_FROM_WIN32(err);
        ThrowForHR(hr, "NetworkIsolationGetAppContainerConfig failed in Enable");
        return hr;
    }

    // Check if already exempted
    bool alreadyExempt = false;
    for (DWORD i = 0; i < curCount; ++i)
    {
        if (EqualSid(targetSid, curList[i].Sid))
        {
            alreadyExempt = true;
            break;
        }
    }

    if (!alreadyExempt)
    {
        // Build new exemption list with target SID appended
        hutao::Array<SID_AND_ATTRIBUTES> newList;
        newList.Reserve(curCount + 1);

        for (DWORD i = 0; i < curCount; ++i)
        {
            newList.Add(curList[i]);
        }

        SID_AND_ATTRIBUTES newEntry;
        newEntry.Sid = targetSid;
        newEntry.Attributes = 0;
        newList.Add(newEntry);

        // Apply the new exemption list
        err = m_pSetAppContainerConfig(static_cast<DWORD>(newList.Length()), newList.Data());
        if (err != ERROR_SUCCESS)
        {
            hr = HRESULT_FROM_WIN32(err);
            ThrowForHR(hr, "NetworkIsolationSetAppContainerConfig failed in Enable");
        }
    }

    // Cleanup GetConfig memory (allocated on process heap per MSDN)
    if (curList)
    {
        HANDLE heap = GetProcessHeap();
        for (DWORD i = 0; i < curCount; ++i)
        {
            if (curList[i].Sid)
            {
                HeapFree(heap, 0, curList[i].Sid);
            }
        }
        HeapFree(heap, 0, curList);
    }

    if (!alreadyExempt && FAILED(hr))
    {
        free(targetSid);
        return hr;
    }

    free(targetSid);
    return S_OK;
}

HRESULT __stdcall HutaoNativeLoopbackSupport::IsPublicFirewallEnabled(boolean* enabled)
{
    AssertNonNullAndReturn(enabled);
    *enabled = false;

    if (!m_firewallPolicy)
    {
        HRESULT hr = CoCreateInstance(
            __uuidof(NetFwPolicy2),
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwPolicy2),
            reinterpret_cast<void**>(&m_firewallPolicy));
        if (FAILED(hr))
        {
            ThrowForHR(hr, "Failed to create NetFwPolicy2 instance");
            return hr;
        }
    }

    VARIANT_BOOL firewallEnabled = VARIANT_FALSE;
    HRESULT hr = m_firewallPolicy->get_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, &firewallEnabled);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "get_FirewallEnabled failed");
        return hr;
    }

    *enabled = (firewallEnabled == VARIANT_TRUE) ? true : false;
    return S_OK;
}
