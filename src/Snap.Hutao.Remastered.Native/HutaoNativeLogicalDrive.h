#pragma once

#include "IHutaoNativeLogicalDrive_h.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeLogicalDrive : public hutao::CustomImplements<HutaoNativeLogicalDrive, IHutaoNativeLogicalDrive>
{
public:
    HutaoNativeLogicalDrive() = default;
    ~HutaoNativeLogicalDrive() = default;

    // IHutaoNativeLogicalDrive methods
    HRESULT STDMETHODCALLTYPE GetDiskFreeSpace(PCWSTR path, long* bytes) noexcept;
};
