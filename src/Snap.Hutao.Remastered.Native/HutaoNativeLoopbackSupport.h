#pragma once

#include "IHutaoNativeLoopbackSupport.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <netfw.h>

class HutaoNativeLoopbackSupport :
    public hutao::CustomImplements<
    HutaoNativeLoopbackSupport,
    IHutaoNativeLoopbackSupport,
    IHutaoNativeLoopbackSupport2>
{
public:
    HutaoNativeLoopbackSupport();
    ~HutaoNativeLoopbackSupport();

    virtual HRESULT __stdcall IsEnabled(PCWSTR familyName, IHutaoString** sid, boolean* enabled) override;
    virtual HRESULT __stdcall Enable(PCWSTR familyName, IHutaoString** sid) override;
    virtual HRESULT __stdcall IsPublicFirewallEnabled(boolean* enabled) override;

private:
    HMODULE m_firewallApiModule;
    INetFwPolicy2* m_firewallPolicy;

    // Dynamically loaded Network Isolation API function pointers
    typedef DWORD(WINAPI* FN_EnumAppContainers)(DWORD, DWORD*, PINET_FIREWALL_APP_CONTAINER*);
    typedef void(WINAPI* FN_FreeAppContainers)(PINET_FIREWALL_APP_CONTAINER);
    typedef DWORD(WINAPI* FN_GetConfig)(DWORD*, PSID_AND_ATTRIBUTES*);
    typedef DWORD(WINAPI* FN_SetConfig)(DWORD, PSID_AND_ATTRIBUTES);

    FN_EnumAppContainers m_pEnumAppContainers;
    FN_FreeAppContainers m_pFreeAppContainers;
    FN_GetConfig m_pGetAppContainerConfig;
    FN_SetConfig m_pSetAppContainerConfig;

    HRESULT LoadNetworkIsolationApi();
    HRESULT FindAppContainerSid(PCWSTR familyName, PSID* outSid);
    HRESULT CreateHutaoStringFromSid(PSID sid, IHutaoString** outString);
};
