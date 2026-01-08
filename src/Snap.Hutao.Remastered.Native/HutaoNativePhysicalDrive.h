#pragma once

#include "IHutaoNativePhysicalDrive.h"
#include "CustomImplements.h"

class HutaoNativePhysicalDrive : public hutao::CustomImplements<HutaoNativePhysicalDrive, IHutaoNativePhysicalDrive>
{
public:
    HutaoNativePhysicalDrive() = default;
    ~HutaoNativePhysicalDrive() = default;

    HRESULT __stdcall IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) noexcept;
};
