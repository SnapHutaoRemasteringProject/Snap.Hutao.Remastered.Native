#pragma once

#include "IHutaoNativeLogicalDrive.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeLogicalDrive : public hutao::CustomImplements<HutaoNativeLogicalDrive, IHutaoNativeLogicalDrive>
{
public:
    HutaoNativeLogicalDrive() = default;
    ~HutaoNativeLogicalDrive() = default;

    // IHutaoNativeLogicalDrive methods
    HRESULT __stdcall GetDiskFreeSpace(PCWSTR path, long* bytes) noexcept;
};
