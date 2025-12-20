#pragma once

#include "IHutaoNativePhysicalDrive_h.h"
#include <winrt/base.h>

class HutaoNativePhysicalDrive : public winrt::implements<HutaoNativePhysicalDrive, IHutaoNativePhysicalDrive, winrt::non_agile>
{
public:
    HutaoNativePhysicalDrive() = default;
    ~HutaoNativePhysicalDrive() = default;

    HRESULT STDMETHODCALLTYPE IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) noexcept;
};
