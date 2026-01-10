#pragma once

#include "IHutaoNativeNotifyIcon.h"
#include "CustomImplements.h"
#include "HutaoNativeNotifyIconCallback.h"
#include "types.h"
#include <Windows.h>
#include <shellapi.h>

class HutaoNativeNotifyIcon : public hutao::CustomImplements<
    HutaoNativeNotifyIcon,
    IHutaoNativeNotifyIcon>
{
public:
    HutaoNativeNotifyIcon(PCWSTR iconPath = nullptr);
    ~HutaoNativeNotifyIcon();

    // IHutaoNativeNotifyIcon methods
    virtual HRESULT __stdcall Create(HutaoNativeNotifyIconCallback callback, GCHandle userData, PCWSTR tip) override;
    virtual HRESULT __stdcall Recreate(PCWSTR tip) override;
    virtual HRESULT __stdcall Destroy() override;
    virtual HRESULT __stdcall IsPromoted(BOOL* promoted) override;

private:
    NOTIFYICONDATAW m_notifyIcoData = { 0 };
    HWND m_hWnd = nullptr;
    UINT m_uCallbackMessage = 0;
    HICON mIcon = nullptr;
    bool m_created = false;
    HutaoNativeNotifyIconCallback m_callback = { };
    intptr_t m_userData = 0;
    wchar_t m_iconPath[MAX_PATH] = { 0 };

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    void HandleNotifyIconMessage(UINT message, WPARAM wParam, LPARAM lParam);
};
