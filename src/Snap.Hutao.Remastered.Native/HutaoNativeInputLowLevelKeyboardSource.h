#pragma once

#include "IHutaoNativeInputLowLevelKeyboardSource_h.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeInputLowLevelKeyboardSource : public hutao::CustomImplements<HutaoNativeInputLowLevelKeyboardSource, IHutaoNativeInputLowLevelKeyboardSource>
{
public:
    HutaoNativeInputLowLevelKeyboardSource() = default;
    ~HutaoNativeInputLowLevelKeyboardSource() = default;

    // IHutaoNativeInputLowLevelKeyboardSource methods
    HRESULT STDMETHODCALLTYPE Attach(nint callback) noexcept;
    HRESULT STDMETHODCALLTYPE Detach(nint callback) noexcept;
};
