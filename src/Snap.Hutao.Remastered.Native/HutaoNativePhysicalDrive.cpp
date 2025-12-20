#include "HutaoNativePhysicalDrive.h"
#include <Windows.h>
#include <winioctl.h>
#include <string>

HRESULT STDMETHODCALLTYPE HutaoNativePhysicalDrive::IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) noexcept
{
    if (root == nullptr || isSSD == nullptr)
    {
        return E_POINTER;
    }

    // Check if path is empty
    if (root[0] == L'\0')
    {
        return E_INVALIDARG;
    }

    // Get drive letter (e.g., "C:\" -> "C:")
    wchar_t drive[4] = { L'C', L':', L'\\', L'\0' };
    if (root[1] == L':')
    {
        drive[0] = root[0];
    }
    else
    {
        // If not a valid drive path, return error
        return E_INVALIDARG;
    }

    // Create drive path
    std::wstring drivePath = L"\\\\.\\";
    drivePath += drive[0];
    drivePath += L":";

    // Open drive
    HANDLE hDrive = CreateFileW(
        drivePath.c_str(),
        0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (hDrive == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Get disk properties
    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    STORAGE_DESCRIPTOR_HEADER header = {};
    DWORD bytesReturned = 0;

    // First get required buffer size
    BOOL result = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query,
        sizeof(query),
        &header,
        sizeof(header),
        &bytesReturned,
        nullptr
    );

    if (!result && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        CloseHandle(hDrive);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Allocate buffer large enough
    DWORD bufferSize = header.Size;
    if (bufferSize == 0)
    {
        bufferSize = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024; // Default size
    }

    BYTE* buffer = new BYTE[bufferSize];
    if (buffer == nullptr)
    {
        CloseHandle(hDrive);
        return E_OUTOFMEMORY;
    }

    // Get full device descriptor
    result = DeviceIoControl(
        hDrive,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query,
        sizeof(query),
        buffer,
        bufferSize,
        &bytesReturned,
        nullptr
    );

    CloseHandle(hDrive);

    if (!result)
    {
        delete[] buffer;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Parse device descriptor
    STORAGE_DEVICE_DESCRIPTOR* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer);
    
    // Check media type
    // SSD is usually DeviceType == 0x0C (SEMICONDUCTOR_DRIVE)
    BOOL isSolidState = FALSE;
    
    // Check device type: SSD is usually 0x0C (SEMICONDUCTOR_DRIVE)
    if (descriptor->DeviceType == 0x0C)
    {
        isSolidState = TRUE;
    }
    // Could also check other flags, but DeviceType is most reliable

    delete[] buffer;
    *isSSD = isSolidState;
    return S_OK;
}
