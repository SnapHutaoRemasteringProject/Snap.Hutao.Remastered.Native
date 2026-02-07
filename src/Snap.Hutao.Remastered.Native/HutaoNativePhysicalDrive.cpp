#include "pch.h"
#include "HutaoNativePhysicalDrive.h"
#include "HutaoString.h"
#include <winioctl.h>

HRESULT __stdcall HutaoNativePhysicalDrive::IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) noexcept
{
    if (root == nullptr || isSSD == nullptr)
    {
        return E_POINTER;
    }

    if (root[0] == L'\0')
    {
        return E_INVALIDARG;
    }

    wchar_t driveLetter = L'\0';
    size_t len = wcslen(root);
    
    if (len >= 2 && root[1] == L':')
    {
        driveLetter = root[0];
    }
    else
    {
        return E_INVALIDARG;
    }

    HutaoString drivePath = L"\\\\.\\";
    wchar_t driveLetterStr[2] = { driveLetter, L'\0' };
    drivePath += driveLetterStr;
    drivePath += L":";

    // Open drive with read access
    HANDLE hDrive = CreateFileW(
        drivePath.Data(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hDrive == INVALID_HANDLE_VALUE)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Method 1: Check device type
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

    // Allocate buffer
    DWORD bufferSize = header.Size;
    if (bufferSize == 0)
    {
        bufferSize = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024;
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

    if (!result)
    {
        delete[] buffer;
        CloseHandle(hDrive);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Parse device descriptor
    STORAGE_DEVICE_DESCRIPTOR* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer);
    
    BOOL isSolidState = FALSE;
    
    // Check 1: Device type - SSD is usually 0x0C (SEMICONDUCTOR_DRIVE)
    // But some SSDs report as 0x00 (unknown) or 0x07 (FILE_DEVICE_DISK)
    if (descriptor->DeviceType == 0x0C) // SEMICONDUCTOR_DRIVE
    {
        isSolidState = TRUE;
    }
    else if (descriptor->DeviceType == 0x00 || descriptor->DeviceType == 0x07)
    {
        // Could be SSD, need additional checks
        // Method 2: Check for seek penalty property
        STORAGE_PROPERTY_QUERY seekPenaltyQuery = {};
        seekPenaltyQuery.PropertyId = StorageDeviceSeekPenaltyProperty;
        seekPenaltyQuery.QueryType = PropertyStandardQuery;
        
        DEVICE_SEEK_PENALTY_DESCRIPTOR seekPenaltyDesc = {};
        DWORD seekBytesReturned = 0;
        
        result = DeviceIoControl(
            hDrive,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &seekPenaltyQuery,
            sizeof(seekPenaltyQuery),
            &seekPenaltyDesc,
            sizeof(seekPenaltyDesc),
            &seekBytesReturned,
            nullptr
        );
        
        if (result)
        {
            // SSDs typically have no seek penalty (IncursSeekPenalty = FALSE)
            // HDDs typically have seek penalty (IncursSeekPenalty = TRUE)
            if (!seekPenaltyDesc.IncursSeekPenalty)
            {
                isSolidState = TRUE;
            }
        }
        
        // Method 3: Check for TRIM support (optional)
        if (!isSolidState)
        {
            STORAGE_PROPERTY_QUERY trimQuery = {};
            trimQuery.PropertyId = StorageDeviceTrimProperty;
            trimQuery.QueryType = PropertyStandardQuery;
            
            DEVICE_TRIM_DESCRIPTOR trimDesc = {};
            DWORD trimBytesReturned = 0;
            
            result = DeviceIoControl(
                hDrive,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &trimQuery,
                sizeof(trimQuery),
                &trimDesc,
                sizeof(trimDesc),
                &trimBytesReturned,
                nullptr
            );
            
            if (result && trimDesc.TrimEnabled)
            {
                // Device supports TRIM, likely an SSD
                isSolidState = TRUE;
            }
        }
    }

    delete[] buffer;
    CloseHandle(hDrive);
    
    *isSSD = isSolidState;
    return S_OK;
}
