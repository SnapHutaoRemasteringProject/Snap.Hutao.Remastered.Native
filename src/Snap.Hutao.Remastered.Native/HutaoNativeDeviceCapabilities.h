#pragma once

#include "IHutaoNativeDeviceCapabilities_h.h"
#include <winrt/base.h>
#include <Windows.h>

class HutaoNativeDeviceCapabilities : public winrt::implements<HutaoNativeDeviceCapabilities, IHutaoNativeDeviceCapabilities, winrt::non_agile>
{
public:
    HutaoNativeDeviceCapabilities() = default;
    ~HutaoNativeDeviceCapabilities() = default;

    // IHutaoNativeDeviceCapabilities methods
    HRESULT STDMETHODCALLTYPE GetPrimaryScreenVerticalRefreshRate(int* refreshRate) noexcept;
};
