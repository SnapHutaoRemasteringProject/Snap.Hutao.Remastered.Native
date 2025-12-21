#pragma once

#include "IHutaoNativeLoopbackSupport_h.h"
#include "FirewallRuleManager.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeLoopbackSupport :
	public hutao::CustomImplements<
	HutaoNativeLoopbackSupport,
	IHutaoNativeLoopbackSupport>
{
public:
	HutaoNativeLoopbackSupport();
	~HutaoNativeLoopbackSupport();

	virtual HRESULT STDMETHODCALLTYPE IsEnabled(PCWSTR familyName, IHutaoString* sid, boolean* enabled) override;
	virtual HRESULT STDMETHODCALLTYPE Enable(PCWSTR familyName, IHutaoString* sid) override;

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

	virtual HRESULT STDMETHODCALLTYPE IsPublicFirewallEnabled(boolean* enabled) override;

private:
	class FirewallRuleManager* m_firewallManager;
};
