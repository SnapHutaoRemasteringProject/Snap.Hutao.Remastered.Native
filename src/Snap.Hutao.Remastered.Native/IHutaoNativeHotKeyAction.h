#pragma once
#include "types.h"
#include <Windows.h>
#include <unknwn.h>

// {8C776674-9910-4721-A764-97BDB791F719}
class __declspec(uuid("8C776674-9910-4721-A764-97BDB791F719")) IHutaoNativeHotKeyAction : public IUnknown
{
public:
    virtual HRESULT __stdcall Update(int modifiers, uint vk) = 0;
    virtual HRESULT __stdcall GetIsEnabled(BOOL* isEnabled) = 0;
    virtual HRESULT __stdcall SetIsEnabled(BOOL isEnabled) = 0;
};
