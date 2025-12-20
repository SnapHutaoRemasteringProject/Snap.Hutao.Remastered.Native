#pragma once

#include "IHutaoNativeInputLowLevelKeyboardSource_h.h"
#include <winrt/base.h>
#include <Windows.h>

class HutaoNativeInputLowLevelKeyboardSource : public winrt::implements<HutaoNativeInputLowLevelKeyboardSource, IHutaoNativeInputLowLevelKeyboardSource, winrt::non_agile>
{
public:
    HutaoNativeInputLowLevelKeyboardSource() = default;
    ~HutaoNativeInputLowLevelKeyboardSource() = default;

    // IHutaoNativeInputLowLevelKeyboardSource methods
    HRESULT STDMETHODCALLTYPE Attach(nint callback) noexcept;
    HRESULT STDMETHODCALLTYPE Detach(nint callback) noexcept;
};
