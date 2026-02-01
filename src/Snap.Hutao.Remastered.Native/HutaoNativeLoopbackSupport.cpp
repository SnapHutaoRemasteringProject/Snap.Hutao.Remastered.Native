#include "pch.h"
#include "HutaoNativeLoopbackSupport.h"
#include "FirewallRuleManager.h"

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

HRESULT __stdcall HutaoNativeLoopbackSupport::IsEnabled(PCWSTR familyName, IHutaoString** sid, boolean* enabled)
{
    AssertNonNullAndReturn(enabled);
    AssertNonNullAndReturn(familyName);
    AssertNonNullAndReturn(sid);

    if (!m_firewallManager)
    {
        HRESULT hr = E_FAIL;
        ThrowForHR(hr, "Firewall manager is null in IsEnabled");
        return hr;
    }

    HutaoString familyNameStr = familyName;

    BOOL nativeEnabled = FALSE;
	HutaoString* sidStr = new HutaoString();

    HRESULT hr = m_firewallManager->IsLoopbackExempt(familyNameStr, *sidStr, &nativeEnabled);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "FirewallRuleManager::IsLoopbackExempt failed in IsEnabled");
        return hr;
    }

    *sid = sidStr;
    *enabled = nativeEnabled ? true : false;
    return S_OK;
}

HRESULT __stdcall HutaoNativeLoopbackSupport::Enable(PCWSTR familyName, IHutaoString** sid)
{
    AssertNonNullAndReturn(familyName);
    AssertNonNullAndReturn(sid);

    if (!m_firewallManager)
    {
        HRESULT hr = E_FAIL;
        ThrowForHR(hr, "Firewall manager is null in Enable");
        return hr;
    }

    HutaoString familyNameStr = familyName;
    HutaoString* sidStr = new HutaoString();

    HRESULT hr = m_firewallManager->AddLoopbackExempt(familyNameStr, *sidStr);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "FirewallRuleManager::AddLoopbackExempt failed in Enable");
    }

    *sid = sidStr;

    return hr;
}

HRESULT __stdcall HutaoNativeLoopbackSupport::IsPublicFirewallEnabled(boolean* enabled)
{
    AssertNonNullAndReturn(enabled);

    if (!m_firewallManager)
    {
        HRESULT hr = E_FAIL;
        ThrowForHR(hr, "Firewall manager is null in IsPublicFirewallEnabled");
        return hr;
    }

    BOOL nativeEnabled = FALSE;
    HRESULT hr = m_firewallManager->IsPublicFirewallEnabled(&nativeEnabled);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "FirewallRuleManager::IsPublicFirewallEnabled failed in IsPublicFirewallEnabled");
        return hr;
    }

    *enabled = nativeEnabled ? true : false;
    return S_OK;
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

HRESULT __stdcall HutaoNativeLoopbackSupport2::IsPublicFirewallEnabled(boolean* enabled)
{
    AssertNonNullAndReturn(enabled);

    if (!m_firewallManager)
    {
        HRESULT hr = E_FAIL;
        ThrowForHR(hr, "Firewall manager is null in IsPublicFirewallEnabled");
        return hr;
    }

    BOOL nativeEnabled = FALSE;
    HRESULT hr = m_firewallManager->IsPublicFirewallEnabled(&nativeEnabled);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "FirewallRuleManager::IsPublicFirewallEnabled failed in IsPublicFirewallEnabled");
        return hr;
    }

    *enabled = nativeEnabled ? true : false;
    return S_OK;
}
