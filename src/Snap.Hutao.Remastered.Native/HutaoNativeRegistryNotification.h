#pragma once

#include "IHutaoNativeRegistryNotification_h.h"
#include "types.h"
#include <winrt/base.h>
#include <Windows.h>
#include <thread>
#include <atomic>

class HutaoNativeRegistryNotification : public winrt::implements<HutaoNativeRegistryNotification, IHutaoNativeRegistryNotification, winrt::non_agile>
{
public:
    HutaoNativeRegistryNotification();
    ~HutaoNativeRegistryNotification();

    // Match the IDL signature: callback is a 32-bit long on Win64
    virtual HRESULT STDMETHODCALLTYPE Start(nint callback, INT64 userData) override;

private:
    static DWORD WINAPI NotificationThreadProc(LPVOID lpParameter);
    void NotificationThread();

    std::thread notificationThread_;
    std::atomic<bool> stopRequested_{false};
    nint callback_ = 0;
    INT64 userData_ = 0;
    HKEY hKey_ = nullptr;
};
