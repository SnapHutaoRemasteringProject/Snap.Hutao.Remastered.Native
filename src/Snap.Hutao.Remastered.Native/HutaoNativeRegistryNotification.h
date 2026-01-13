#pragma once

#include "IHutaoNativeRegistryNotification.h"
#include "HutaoNativeRegistryNotificationCallBack.h"
#include "types.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <thread>
#include <atomic>

class HutaoNativeRegistryNotification : public hutao::CustomImplements<HutaoNativeRegistryNotification, IHutaoNativeRegistryNotification>
{
public:
    HutaoNativeRegistryNotification(HutaoString keyPath);
    ~HutaoNativeRegistryNotification();

    virtual HRESULT __stdcall Start(HutaoNativeRegistryNotificationCallBack callback, GCHandle userData) override;

private:
    static DWORD WINAPI NotificationThreadProc(LPVOID lpParameter);
    void NotificationThread();

    std::thread notificationThread_;
    std::atomic<bool> stopRequested_{false};
    HutaoNativeRegistryNotificationCallBack callback_;
    GCHandle userData_ = 0;
    HKEY hKey_ = nullptr;

    // Store the registry key path passed from managed code
    HutaoString keyPath_;

    // Parse a registry path like "HKEY_CURRENT_USER\\Software\\..." or "HKCU\\Software\\..."
    // Returns true on success and fills outRootKey and outSubKey (outSubKey may be empty)
    bool ParseRegistryPath(const HutaoString& path, HKEY& outRootKey, HutaoString& outSubKey);
};
