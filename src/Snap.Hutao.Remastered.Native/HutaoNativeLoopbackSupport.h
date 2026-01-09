#pragma once

#include "IHutaoNativeLoopbackSupport.h"
#include "FirewallRuleManager.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeLoopbackSupport :
	public hutao::CustomImplements<
	HutaoNativeLoopbackSupport,
	IHutaoNativeLoopbackSupport,
	IHutaoNativeLoopbackSupport2>
{
public:
	HutaoNativeLoopbackSupport();
	~HutaoNativeLoopbackSupport();

	virtual HRESULT __stdcall IsEnabled(PCWSTR familyName, IHutaoString* sid, boolean* enabled) override;
	virtual HRESULT __stdcall Enable(PCWSTR familyName, IHutaoString* sid) override;
	virtual HRESULT __stdcall IsPublicFirewallEnabled(boolean* enabled) override;

private:
	class FirewallRuleManager* m_firewallManager;
};

class HutaoNativeLoopbackSupport2 :
	public hutao::CustomImplements<
	HutaoNativeLoopbackSupport2,
	IHutaoNativeLoopbackSupport2>
{
public:
	HutaoNativeLoopbackSupport2();
	~HutaoNativeLoopbackSupport2();

	virtual HRESULT __stdcall IsPublicFirewallEnabled(boolean* enabled) override;

private:
	class FirewallRuleManager* m_firewallManager;
};
