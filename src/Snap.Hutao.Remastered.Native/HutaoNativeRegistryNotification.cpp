#include "HutaoNativeRegistryNotification.h"
#include "types.h"
#include <string>
#include <vector>

HutaoNativeRegistryNotification::HutaoNativeRegistryNotification()
    : callback_(0)
    , userData_(0)
    , hKey_(nullptr)
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

void HutaoNativeRegistryNotification::NotificationThread()
{
    // Parse registry path from C# code: HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections
    // We need to open HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings
    // and watch for changes in the Connections subkey
    
    HKEY hRootKey = HKEY_CURRENT_USER;
    std::wstring subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
    
    LONG result = RegOpenKeyExW(hRootKey, subKey.c_str(), 0, KEY_NOTIFY | KEY_READ, &hKey_);
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
        // Watch for changes in the Connections subkey
        // We use REG_NOTIFY_CHANGE_LAST_SET to detect value changes
        // and REG_NOTIFY_CHANGE_NAME to detect subkey changes
        // Set bWatchSubtree to TRUE to monitor the Connections subkey as well
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
            if (callback_ != 0)
            {
                // The callback is a function pointer with signature: void (__stdcall *)(nint userData)
                using CallbackType = void(__stdcall*)(nint);
                CallbackType callbackFunc = reinterpret_cast<CallbackType>(callback_);
                
                // Call the callback function
                if (callbackFunc != nullptr)
                {
                    callbackFunc((nint)userData_);
                }
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

HRESULT STDMETHODCALLTYPE HutaoNativeRegistryNotification::Start(nint callback, INT64 userData) {
    if (!callback)
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
