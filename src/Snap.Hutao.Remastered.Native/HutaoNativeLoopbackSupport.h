#pragma once

#include "IHutaoNativeLoopbackSupport_h.h"
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

	virtual HRESULT STDMETHODCALLTYPE IsEnabled(PCWSTR familyName, IHutaoString* sid, boolean* enabled) override;
	virtual HRESULT STDMETHODCALLTYPE Enable(PCWSTR familyName, IHutaoString* sid) override;
	virtual HRESULT STDMETHODCALLTYPE IsPublicFirewallEnabled(boolean* enabled) override;

private:
	class FirewallRuleManager* m_firewallManager;
};
