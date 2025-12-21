#pragma once

#include "IHutaoNativePhysicalDrive_h.h"
#include "CustomImplements.h"

class HutaoNativePhysicalDrive : public hutao::CustomImplements<HutaoNativePhysicalDrive, IHutaoNativePhysicalDrive>
{
public:
    HutaoNativePhysicalDrive() = default;
    ~HutaoNativePhysicalDrive() = default;

    HRESULT STDMETHODCALLTYPE IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) noexcept;
};
