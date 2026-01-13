#include "pch.h"
#include "HutaoNativeRegistryNotification.h"
#include "HutaoNativeRegistryNotificationCallBack.h"
#include <algorithm>

HutaoNativeRegistryNotification::HutaoNativeRegistryNotification(HutaoString keyPath)
    : callback_(0)
    , userData_(0)
    , hKey_(nullptr)
    , keyPath_(keyPath)
{
}

HutaoNativeRegistryNotification::~HutaoNativeRegistryNotification()
{
    stopRequested_ = true;
    
    // Close registry key if open
    if (hKey_ != nullptr)
    {
        RegCloseKey(hKey_);
        hKey_ = nullptr;
    }
    
    // Wait for thread to finish
    if (notificationThread_.joinable())
    {
        notificationThread_.join();
    }
}

DWORD WINAPI HutaoNativeRegistryNotification::NotificationThreadProc(LPVOID lpParameter)
{
    HutaoNativeRegistryNotification* instance = static_cast<HutaoNativeRegistryNotification*>(lpParameter);
    if (instance)
    {
        instance->NotificationThread();
    }
    return 0;
}

bool HutaoNativeRegistryNotification::ParseRegistryPath(const HutaoString& path, HKEY& outRootKey, HutaoString& outSubKey)
{
    outRootKey = nullptr;
    outSubKey = HutaoString();

    // Trim and normalize case using HutaoString methods
    HutaoString trimmed = path.Trim();
    if (trimmed.IsEmpty())
    {
        return false;
    }

    // Find separator
    int idx = trimmed.IndexOf(L'\\');
    HutaoString hiveName;
    if (idx < 0)
    {
        hiveName = trimmed;
        outSubKey = HutaoString();
    }
    else
    {
        hiveName = trimmed.Substring(0, static_cast<size_t>(idx));
        outSubKey = trimmed.Substring(static_cast<size_t>(idx + 1));
    }

    HutaoString hiveUpper = hiveName.ToUpper();

    if (hiveUpper == L"HKEY_CLASSES_ROOT" || hiveUpper == L"HKCR")
    {
        outRootKey = HKEY_CLASSES_ROOT;
    }
    else if (hiveUpper == L"HKEY_CURRENT_USER" || hiveUpper == L"HKCU")
    {
        outRootKey = HKEY_CURRENT_USER;
    }
    else if (hiveUpper == L"HKEY_LOCAL_MACHINE" || hiveUpper == L"HKLM")
    {
        outRootKey = HKEY_LOCAL_MACHINE;
    }
    else if (hiveUpper == L"HKEY_USERS" || hiveUpper == L"HKU")
    {
        outRootKey = HKEY_USERS;
    }
    else if (hiveUpper == L"HKEY_CURRENT_CONFIG" || hiveUpper == L"HKCC")
    {
        outRootKey = HKEY_CURRENT_CONFIG;
    }
    else
    {
        return false;
    }

    return true;
}

void HutaoNativeRegistryNotification::NotificationThread()
{
    HKEY root = HKEY_CURRENT_USER;
    HutaoString subKey;
    if (!ParseRegistryPath(keyPath_, root, subKey))
    {
        return;
    }

    // Open the target key (subKey may be empty meaning the root hive)
    LONG result = RegOpenKeyExW(root, subKey.IsEmpty() ? nullptr : subKey.Data(), 0, KEY_NOTIFY | KEY_READ, &hKey_);
    if (result != ERROR_SUCCESS)
    {
        // Failed to open registry key
        return;
    }
    
    // Create an event for registry notification
    HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (hEvent == nullptr)
    {
        RegCloseKey(hKey_);
        hKey_ = nullptr;
        return;
    }
    
    while (!stopRequested_)
    {
        // Watch for changes in the key and its subkeys
        result = RegNotifyChangeKeyValue(
            hKey_,                     // hKey
            TRUE,                      // bWatchSubtree (TRUE = this key and all subkeys)
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME, // dwNotifyFilter
            hEvent,                    // hEvent
            TRUE                       // fAsynchronous (TRUE = returns immediately)
        );
        
        if (result != ERROR_SUCCESS)
        {
            // Error occurred, break out of loop
            break;
        }
        
        // Wait for change with timeout to allow checking stopRequested_
        DWORD waitResult = WaitForSingleObject(hEvent, 1000);
        
        if (waitResult == WAIT_OBJECT_0)
        {
            // Registry changed, call the callback
            if (callback_.has_value())
            {
                callback_.value()(userData_);
            }
        }
        else if (waitResult == WAIT_TIMEOUT)
        {
            // Timeout, check if we should stop
            continue;
        }
        else
        {
            // Error occurred
            break;
        }
    }
    
    // Cleanup
    CloseHandle(hEvent);
    
    if (hKey_ != nullptr)
    {
        RegCloseKey(hKey_);
        hKey_ = nullptr;
    }
}

HRESULT __stdcall HutaoNativeRegistryNotification::Start(HutaoNativeRegistryNotificationCallBack callback, GCHandle userData) {
    if (!callback.has_value())
    {
        return E_INVALIDARG;
    }
    
    // Store callback and user data
    callback_ = callback;
    userData_ = userData;
    
    // Start the notification thread
    try
    {
        notificationThread_ = std::thread(&HutaoNativeRegistryNotification::NotificationThread, this);
        return S_OK;
    }
    catch (...)
    {
        return E_FAIL;
    }
}
