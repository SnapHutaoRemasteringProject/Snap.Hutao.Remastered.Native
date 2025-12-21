#include "HutaoNativeNotifyIcon.h"
#include <Windows.h>
#include <shellapi.h>
#include <string>

namespace
{
    const wchar_t* WINDOW_CLASS_NAME = L"HutaoNativeNotifyIconWindowClass";
    const UINT WM_NOTIFYICON_CALLBACK = WM_APP + 1;
}

HutaoNativeNotifyIcon::HutaoNativeNotifyIcon()
    : m_hWnd(nullptr)
    , m_uCallbackMessage(WM_NOTIFYICON_CALLBACK)
    , m_hIcon(nullptr)
    , m_created(false)
    , m_callback(nullptr)
    , m_userData(0)
{
    ZeroMemory(&m_notifyIconData, sizeof(m_notifyIconData));
    m_notifyIconData.cbSize = sizeof(m_notifyIconData);
}

HutaoNativeNotifyIcon::~HutaoNativeNotifyIcon()
{
    Destroy();
}

ATOM HutaoNativeNotifyIcon::RegisterWindowClass()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &HutaoNativeNotifyIcon::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    
    return RegisterClassExW(&wc);
}

HWND HutaoNativeNotifyIcon::CreateMessageWindow()
{
    static ATOM classAtom = RegisterWindowClass();
    if (classAtom == 0)
    {
        return nullptr;
    }

    HWND hWnd = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"HutaoNativeNotifyIconMessageWindow",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this // 将this指针传递给窗口
    );

    return hWnd;
}

LRESULT CALLBACK HutaoNativeNotifyIcon::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        return 0;
    }

    HutaoNativeNotifyIcon* pThis = reinterpret_cast<HutaoNativeNotifyIcon*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (pThis != nullptr)
    {
        if (message == pThis->m_uCallbackMessage)
        {
            pThis->HandleNotifyIconMessage(message, wParam, lParam);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

void HutaoNativeNotifyIcon::HandleNotifyIconMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_callback != nullptr)
    {
        // 调用回调函数
        m_callback(reinterpret_cast<HWND>(m_hWnd), message, wParam, lParam);
    }
}

HRESULT STDMETHODCALLTYPE HutaoNativeNotifyIcon::Create(nint callback, nint userData, PCWSTR tip)
{
    if (m_created)
    {
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    m_callback = reinterpret_cast<WNDPROC>(callback);
    m_userData = userData;

    // 创建消息窗口
    m_hWnd = CreateMessageWindow();
    if (m_hWnd == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // 设置通知图标数据
    m_notifyIconData.hWnd = m_hWnd;
    m_notifyIconData.uFlags = NIF_MESSAGE | NIF_TIP;
    m_notifyIconData.uCallbackMessage = m_uCallbackMessage;
    
    if (tip != nullptr)
    {
        wcsncpy_s(m_notifyIconData.szTip, tip, _countof(m_notifyIconData.szTip) - 1);
        m_notifyIconData.szTip[_countof(m_notifyIconData.szTip) - 1] = L'\0';
    }

    // 添加通知图标
    if (!Shell_NotifyIconW(NIM_ADD, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    m_created = true;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeNotifyIcon::Recreate(PCWSTR tip)
{
    if (!m_created)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    // 先删除现有的通知图标
    if (!Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // 更新提示文本
    if (tip != nullptr)
    {
        wcsncpy_s(m_notifyIconData.szTip, tip, _countof(m_notifyIconData.szTip) - 1);
        m_notifyIconData.szTip[_countof(m_notifyIconData.szTip) - 1] = L'\0';
    }

    // 重新添加通知图标
    if (!Shell_NotifyIconW(NIM_ADD, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeNotifyIcon::Destroy()
{
    if (!m_created)
    {
        return S_OK;
    }

    // 删除通知图标
    if (!Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData))
    {
        // 即使删除失败，我们也继续清理
    }

    // 销毁窗口
    if (m_hWnd != nullptr)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }

    // 销毁图标
    if (m_hIcon != nullptr)
    {
        DestroyIcon(m_hIcon);
        m_hIcon = nullptr;
    }

    m_created = false;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeNotifyIcon::IsPromoted(BOOL* promoted)
{
    if (promoted == nullptr)
    {
        return E_POINTER;
    }

    // 简单实现：总是返回FALSE
    // 实际实现应该检查通知图标是否被提升（例如，在Windows 11中）
    *promoted = FALSE;
    return S_OK;
}
