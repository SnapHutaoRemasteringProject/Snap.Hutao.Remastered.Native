#pragma once

#include "IHutaoNativeDeviceCapabilities_h.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeDeviceCapabilities : public hutao::CustomImplements<HutaoNativeDeviceCapabilities, IHutaoNativeDeviceCapabilities>
{
public:
    HutaoNativeDeviceCapabilities() = default;
    ~HutaoNativeDeviceCapabilities() = default;

    // IHutaoNativeDeviceCapabilities methods
    HRESULT STDMETHODCALLTYPE GetPrimaryScreenVerticalRefreshRate(int* refreshRate) noexcept;
};
