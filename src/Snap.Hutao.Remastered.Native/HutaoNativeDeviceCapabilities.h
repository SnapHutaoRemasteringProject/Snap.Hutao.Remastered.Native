#pragma once

#include "IHutaoNativeDeviceCapabilities.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeDeviceCapabilities : public hutao::CustomImplements<HutaoNativeDeviceCapabilities, IHutaoNativeDeviceCapabilities>
{
public:
    HutaoNativeDeviceCapabilities() = default;
    ~HutaoNativeDeviceCapabilities() = default;

    // IHutaoNativeDeviceCapabilities methods
    HRESULT __stdcall GetPrimaryScreenVerticalRefreshRate(int* refreshRate) noexcept;
};
