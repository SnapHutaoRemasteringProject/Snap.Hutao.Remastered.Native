#pragma once

#include "IHutaoNativeNotifyIcon_h.h"
#include "CustomImplements.h"
#include "HutaoNativeNotifyIconCallback.h"
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
    virtual HRESULT STDMETHODCALLTYPE Create(nint callback, nint userData, PCWSTR tip) override;
    virtual HRESULT STDMETHODCALLTYPE Recreate(PCWSTR tip) override;
    virtual HRESULT STDMETHODCALLTYPE Destroy() override;
    virtual HRESULT STDMETHODCALLTYPE IsPromoted(BOOL* promoted) override;

private:
    NOTIFYICONDATAW m_notifyIconData = { 0 };
    HWND m_hWnd = nullptr;
    UINT m_uCallbackMessage = 0;
    HICON m_hIcon = nullptr;
    bool m_created = false;
    HutaoNativeNotifyIconCallback m_callback = { nullptr };
    nint m_userData = 0;
    wchar_t m_iconPath[MAX_PATH] = { 0 };

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    void HandleNotifyIconMessage(UINT message, WPARAM wParam, LPARAM lParam);
};
