#pragma once

#include <Windows.h>
#include <unknwn.h>

// {1920EFA1-9953-4260-AFB1-35B1672758C1}
class __declspec(uuid("1920EFA1-9953-4260-AFB1-35B1672758C1")) IHutaoNativeDeviceCapabilities : public IUnknown
{
public:
    virtual HRESULT __stdcall GetPrimaryScreenVerticalRefreshRate(int* refreshRate) = 0;
};
