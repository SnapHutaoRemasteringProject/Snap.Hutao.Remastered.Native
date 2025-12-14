#pragma once

#include "HutaoNativeLoopbackSupport.h"
#include "FirewallRuleManager.h"
#include <Windows.h>
#include <string>

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

HRESULT WINAPI HutaoNativeLoopbackSupport::IsEnabled(PCWSTR familyName, PCWSTR sid, BOOL* enabled)
{
    if (!familyName || !enabled)
        return E_INVALIDARG;

    if (!m_firewallManager)
        return E_FAIL;

    std::wstring familyNameStr = familyName;
    std::wstring sidStr = sid ? sid : L"";

    return m_firewallManager->IsLoopbackExempt(familyNameStr, sidStr, enabled);
}

HRESULT WINAPI HutaoNativeLoopbackSupport::Enable(PCWSTR familyName, PCWSTR sid)
{
    if (!familyName)
        return E_INVALIDARG;

    if (!m_firewallManager)
        return E_FAIL;

    std::wstring familyNameStr = familyName;
    std::wstring sidStr = sid ? sid : L"";

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

HRESULT WINAPI HutaoNativeLoopbackSupport2::IsPublicFirewallEnabled(BOOL* enabled)
{
    if (!enabled)
        return E_POINTER;

    if (!m_firewallManager)
        return E_FAIL;

    return m_firewallManager->IsPublicFirewallEnabled(enabled);
}