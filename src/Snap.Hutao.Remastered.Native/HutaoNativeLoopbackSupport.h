#pragma once

#include "IHutaoNativeLoopbackSupport_h.h"
#include "FirewallRuleManager.h"
#include <Windows.h>
#include <winrt/base.h>

class HutaoNativeLoopbackSupport :
	public winrt::implements<
	HutaoNativeLoopbackSupport,
	IHutaoNativeLoopbackSupport,
	winrt::non_agile>
{
public:
	HutaoNativeLoopbackSupport();
	~HutaoNativeLoopbackSupport();

	virtual HRESULT STDMETHODCALLTYPE IsEnabled(HSTRING familyName, IHutaoString* sid, boolean* enabled) override;
	virtual HRESULT STDMETHODCALLTYPE Enable(HSTRING familyName, IHutaoString* sid) override;

private:
	class FirewallRuleManager* m_firewallManager;
};

class HutaoNativeLoopbackSupport2 :
	public winrt::implements<
	HutaoNativeLoopbackSupport2,
	IHutaoNativeLoopbackSupport2,
	winrt::non_agile>
{
public:
	HutaoNativeLoopbackSupport2();
	~HutaoNativeLoopbackSupport2();

	virtual HRESULT STDMETHODCALLTYPE IsPublicFirewallEnabled(boolean* enabled) override;

private:
	class FirewallRuleManager* m_firewallManager;
};
