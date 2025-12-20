#pragma once

#include "IHutaoNativeLogicalDrive_h.h"
#include <winrt/base.h>
#include <Windows.h>

class HutaoNativeLogicalDrive : public winrt::implements<HutaoNativeLogicalDrive, IHutaoNativeLogicalDrive, winrt::non_agile>
{
public:
    HutaoNativeLogicalDrive() = default;
    ~HutaoNativeLogicalDrive() = default;

    // IHutaoNativeLogicalDrive methods
    HRESULT STDMETHODCALLTYPE GetDiskFreeSpace(PCWSTR path, long* bytes) noexcept;
};
