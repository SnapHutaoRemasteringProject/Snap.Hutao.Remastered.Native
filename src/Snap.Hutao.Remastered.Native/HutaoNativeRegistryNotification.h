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
    HutaoNativeRegistryNotification();
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
};
