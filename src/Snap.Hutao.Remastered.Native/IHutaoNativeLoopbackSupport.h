#pragma once
#include <Windows.h>
#include <unknwn.h>
#include "IHutaoString.h"

// {8607ACE4-313C-4C26-B1FB-CA11173B6953}
class __declspec(uuid("8607ACE4-313C-4C26-B1FB-CA11173B6953")) IHutaoNativeLoopbackSupport : public IUnknown
{
public:
    virtual HRESULT __stdcall IsEnabled(PCWSTR familyName, IHutaoString** sid, boolean* enabled) = 0;
    virtual HRESULT __stdcall Enable(PCWSTR familyName, IHutaoString** sid) = 0;
};

// {A5D67980-F495-4F52-BEF0-27D047E20174}
class __declspec(uuid("A5D67980-F495-4F52-BEF0-27D047E20174")) IHutaoNativeLoopbackSupport2 : public IUnknown
{
public:
    virtual HRESULT __stdcall IsPublicFirewallEnabled(boolean* enabled) = 0;
};
