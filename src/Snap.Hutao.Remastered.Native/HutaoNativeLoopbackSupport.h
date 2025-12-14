#pragma once

#include "IHutaoNativeLoopbackSupport_h.h"
#include "IHutaoNativeLoopbackSupport2_h.h"
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

	virtual HRESULT WINAPI IsEnabled(PCWSTR familyName, PCWSTR sid, BOOL* enabled);
	virtual HRESULT WINAPI Enable(PCWSTR familyName, PCWSTR sid);

private:
	class FirewallRuleManager* m_firewallManager;
};

// IHutaoNativeLoopbackSupport2  µœ÷¿‡
class HutaoNativeLoopbackSupport2 :
	public winrt::implements<
	HutaoNativeLoopbackSupport2,
	IHutaoNativeLoopbackSupport2,
	winrt::non_agile>
{
public:
	HutaoNativeLoopbackSupport2();
	~HutaoNativeLoopbackSupport2();

	virtual HRESULT WINAPI IsPublicFirewallEnabled(BOOL* enabled);

private:
	class FirewallRuleManager* m_firewallManager;
};
