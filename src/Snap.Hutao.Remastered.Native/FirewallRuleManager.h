#pragma once

#include <Windows.h>
#include <netfw.h>
#include <icftypes.h>
#include "HutaoString.h"
#include "Array.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "advapi32.lib")

class FirewallRuleManager
{
public:
    FirewallRuleManager();
    ~FirewallRuleManager();

    HRESULT Initialize();
    HRESULT IsLoopbackExempt(const HutaoString& familyName, const HutaoString& sid, BOOL* enabled);
    HRESULT AddLoopbackExempt(const HutaoString& familyName, const HutaoString& sid);
    HRESULT IsPublicFirewallEnabled(BOOL* enabled);

private:
    INetFwPolicy2* m_policy;
    INetFwRules* m_rules;
    bool m_initialized;
    HRESULT CreateFirewallRule(const HutaoString& ruleName, const HutaoString& appPath,
        const HutaoString& sid, NET_FW_RULE_DIRECTION direction,
        NET_FW_ACTION action);
    HutaoString GetRuleName(const HutaoString& familyName, const HutaoString& sid);
    HutaoString GetCurrentUserSid();

    // 安全释放COM对象
    template<typename T>
    void SafeRelease(T*& p)
    {
        if (p)
        {
            p->Release();
            p = nullptr;
        }
    }
};
