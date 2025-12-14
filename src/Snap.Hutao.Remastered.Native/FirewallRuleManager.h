#pragma once

#include <Windows.h>
#include <netfw.h>
#include <string>
#include <icftypes.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "advapi32.lib")

class FirewallRuleManager
{
public:
    FirewallRuleManager();
    ~FirewallRuleManager();

    HRESULT Initialize();
    HRESULT IsLoopbackExempt(const std::wstring& familyName, const std::wstring& sid, BOOL* enabled);
    HRESULT AddLoopbackExempt(const std::wstring& familyName, const std::wstring& sid);
    HRESULT IsPublicFirewallEnabled(BOOL* enabled);

private:
    INetFwPolicy2* m_policy;
    INetFwRules* m_rules;
    bool m_initialized;
    HRESULT CreateFirewallRule(const std::wstring& ruleName, const std::wstring& appPath,
        const std::wstring& sid, NET_FW_RULE_DIRECTION direction,
        NET_FW_ACTION action);
    std::wstring GetRuleName(const std::wstring& familyName, const std::wstring& sid);
    std::wstring GetCurrentUserSid();

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