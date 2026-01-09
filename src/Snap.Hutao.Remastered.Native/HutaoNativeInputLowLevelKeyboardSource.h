#pragma once

#include "IHutaoNativeInputLowLevelKeyboardSource.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeInputLowLevelKeyboardSource : public hutao::CustomImplements<HutaoNativeInputLowLevelKeyboardSource, IHutaoNativeInputLowLevelKeyboardSource>
{
public:
    HutaoNativeInputLowLevelKeyboardSource() = default;
    ~HutaoNativeInputLowLevelKeyboardSource() = default;

    // IHutaoNativeInputLowLevelKeyboardSource methods
    HRESULT __stdcall Attach(intptr_t callback) noexcept;
    HRESULT __stdcall Detach(intptr_t callback) noexcept;
};
