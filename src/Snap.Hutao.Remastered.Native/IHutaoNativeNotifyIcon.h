#pragma once
#include <Windows.h>
#include <unknwn.h>
#include "types.h"
#include "IHutaoString.h"
#include "HutaoNativeNotifyIconCallback.h"

// {C2203FA4-BB97-47E0-8F72-C96879E4CB11}
class __declspec(uuid("C2203FA4-BB97-47E0-8F72-C96879E4CB11")) IHutaoNativeNotifyIcon : public IUnknown
{
public:
    virtual HRESULT __stdcall Create(HutaoNativeNotifyIconCallback callback, GCHandle userData, PCWSTR tip) = 0;
    virtual HRESULT __stdcall Recreate(PCWSTR tip) = 0;
    virtual HRESULT __stdcall Destroy() = 0;
    virtual HRESULT __stdcall IsPromoted(BOOL* promoted) = 0;
};
