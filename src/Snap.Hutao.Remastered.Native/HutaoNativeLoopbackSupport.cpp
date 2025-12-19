#pragma once

#include "HutaoNativeLoopbackSupport.h"
#include "FirewallRuleManager.h"
#include <Windows.h>
#include <winstring.h>
#include <string>
#include <winrt/base.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;

HutaoNativeLoopbackSupport::HutaoNativeLoopbackSupport()
    : m_firewallManager(new FirewallRuleManager())
{
}

HutaoNativeLoopbackSupport::~HutaoNativeLoopbackSupport()
{
    if (m_firewallManager)
    {
        delete m_firewallManager;
        m_firewallManager = nullptr;
    }
}

HRESULT STDMETHODCALLTYPE HutaoNativeLoopbackSupport::IsEnabled(HSTRING familyName, IHutaoString* sid, boolean* enabled)
{
    if (enabled == nullptr)
    {
        return E_POINTER;
    }

    if (familyName == nullptr)
    {
        return E_INVALIDARG;
    }

    if (!m_firewallManager)
    {
        return E_FAIL;
    }

    // Convert HSTRING to PCWSTR using WindowsGetStringRawBuffer
    PCWSTR familyNameRaw = WindowsGetStringRawBuffer(familyName, nullptr);

    LPCWSTR sidBuffer = nullptr;
    if (sid)
    {
        HRESULT hr = sid->GetBuffer(&sidBuffer);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    std::wstring familyNameStr = familyNameRaw ? familyNameRaw : L"";
    std::wstring sidStr = sidBuffer ? sidBuffer : L"";

    BOOL nativeEnabled = FALSE;
    HRESULT hr = m_firewallManager->IsLoopbackExempt(familyNameStr, sidStr, &nativeEnabled);
    if (FAILED(hr))
    {
        return hr;
    }

    *enabled = nativeEnabled ? true : false;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeLoopbackSupport::Enable(HSTRING familyName, IHutaoString* sid)
{
    if (familyName == nullptr)
    {
        return E_INVALIDARG;
    }

    if (!m_firewallManager)
    {
        return E_FAIL;
    }

    PCWSTR familyNameRaw = WindowsGetStringRawBuffer(familyName, nullptr);

    LPCWSTR sidBuffer = nullptr;
    if (sid)
    {
        HRESULT hr = sid->GetBuffer(&sidBuffer);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    std::wstring familyNameStr = familyNameRaw ? familyNameRaw : L"";
    std::wstring sidStr = sidBuffer ? sidBuffer : L"";

    return m_firewallManager->AddLoopbackExempt(familyNameStr, sidStr);
}

HutaoNativeLoopbackSupport2::HutaoNativeLoopbackSupport2()
    : m_firewallManager(new FirewallRuleManager())
{
}

HutaoNativeLoopbackSupport2::~HutaoNativeLoopbackSupport2()
{
    if (m_firewallManager)
    {
        delete m_firewallManager;
        m_firewallManager = nullptr;
    }
}

HRESULT STDMETHODCALLTYPE HutaoNativeLoopbackSupport2::IsPublicFirewallEnabled(boolean* enabled)
{
    if (enabled == nullptr)
    {
        return E_POINTER;
    }

    if (!m_firewallManager)
    {
        return E_FAIL;
    }

    BOOL nativeEnabled = FALSE;
    HRESULT hr = m_firewallManager->IsPublicFirewallEnabled(&nativeEnabled);
    if (FAILED(hr))
    {
        return hr;
    }

    *enabled = nativeEnabled ? true : false;
    return S_OK;
}