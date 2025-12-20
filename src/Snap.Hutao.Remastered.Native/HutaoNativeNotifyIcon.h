#pragma once

#include "IHutaoNativeNotifyIcon_h.h"
#include <winrt/base.h>
#include <Windows.h>
#include <shellapi.h>

class HutaoNativeNotifyIcon : public winrt::implements<
    HutaoNativeNotifyIcon,
    IHutaoNativeNotifyIcon,
    winrt::non_agile>
{
public:
    HutaoNativeNotifyIcon();
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
    WNDPROC m_callback = nullptr;
    nint m_userData = 0;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    void HandleNotifyIconMessage(UINT message, WPARAM wParam, LPARAM lParam);
};
