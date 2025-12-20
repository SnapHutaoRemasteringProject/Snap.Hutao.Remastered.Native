#include "HutaoNativeLogicalDrive.h"
#include <Windows.h>

HRESULT STDMETHODCALLTYPE HutaoNativeLogicalDrive::GetDiskFreeSpace(PCWSTR path, long* bytes) noexcept
{
    if (bytes == nullptr)
    {
        return E_POINTER;
    }

    if (path == nullptr)
    {
        return E_INVALIDARG;
    }

    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL success = GetDiskFreeSpaceExW(
        path,
        &freeBytesAvailableToCaller,
        &totalNumberOfBytes,
        &totalNumberOfFreeBytes
    );

    if (!success)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // 返回可用字节数（以字节为单位）
    // 注意：long 是32位，但磁盘空间可能超过32位
    // 这里我们返回可用字节数，如果超过32位则截断
    *bytes = static_cast<long>(freeBytesAvailableToCaller.QuadPart);

    // 明确标记未使用的变量以避免警告
    (void)totalNumberOfBytes;
    (void)totalNumberOfFreeBytes;

    return S_OK;
}
