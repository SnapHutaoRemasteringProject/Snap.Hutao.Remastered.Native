#include "pch.h"
#include "HutaoNativeNotifyIcon.h"
#include <shellapi.h>

namespace
{
    const wchar_t* WINDOW_CLASS_NAME = L"HutaoNativeNotifyIconWindowClass";
    const UINT WM_NOTIFYICON_CALLBACK = WM_APP + 1;
}

HutaoNativeNotifyIcon::HutaoNativeNotifyIcon(PCWSTR iconPath)
    : mWnd(nullptr)
    , m_uCallbackMessage(WM_NOTIFYICON_CALLBACK)
    , mIcon(nullptr)
    , m_created(false)
    , m_callback({ nullptr })
    , m_userData(0)
{
    ZeroMemory(&m_notifyIconData, sizeof(m_notifyIconData));
    m_notifyIconData.cbSize = sizeof(m_notifyIconData);
    
    // 存储图标路径
    if (iconPath != nullptr)
    {
        wcsncpy_s(m_iconPath, iconPath, _countof(m_iconPath) - 1);
        m_iconPath[_countof(m_iconPath) - 1] = L'\0';
    }
    else
    {
        m_iconPath[0] = L'\0';
    }
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
	UINT WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
    if (!m_callback.has_value()) {
        return;
    }

    HutaoNativeNotifyIconCallbackKind kind;
    bool hasKind = false;
    RECT rect = { 0 };
    POINT point = { 0 };

    if (lParam == WM_TASKBARCREATED) {
        kind = HutaoNativeNotifyIconCallbackKind::TaskbarCreated;
		hasKind = true;
    }
    else if (lParam == WM_RBUTTONUP) {
		kind = HutaoNativeNotifyIconCallbackKind::ContextMenu;
        hasKind = true;
    }
    else if (lParam == WM_LBUTTONDOWN) {
        kind = HutaoNativeNotifyIconCallbackKind::LeftButtonDown;
        hasKind = true;
    }
    else if (lParam == WM_LBUTTONDBLCLK)
    {
        kind = HutaoNativeNotifyIconCallbackKind::LeftButtonDoubleClick;
        hasKind = true;
    }

	if (!hasKind)
    {
        return;
    }

    GetCursorPos(&point);

    rect.left = point.x - 8;
    rect.top = point.y - 8;
    rect.right = point.x + 8;
    rect.bottom = point.y + 8;

    m_callback.value()(kind, rect, point, m_userData);
}

HRESULT __stdcall HutaoNativeNotifyIcon::Create(HutaoNativeNotifyIconCallback callback, GCHandle userData, PCWSTR tip)
{
    if (m_created)
    {
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }

    m_callback = callback;
    m_userData = userData;

    // 创建消息窗口
    mWnd = CreateMessageWindow();
    if (mWnd == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (m_iconPath[0] != L'\0')
    {
        mIcon = static_cast<HICON>(LoadImageW(
            nullptr,
            m_iconPath,
            IMAGE_ICON,
            0, 0,
            LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED
        ));
        
        if (mIcon == nullptr)
        {
            mIcon = static_cast<HICON>(LoadImageW(
                GetModuleHandle(nullptr),
                m_iconPath,
                IMAGE_ICON,
                0, 0,
                LR_DEFAULTSIZE | LR_SHARED
            ));
        }
    }

    m_notifyIconData.hWnd = mWnd;
    m_notifyIconData.uFlags = NIF_MESSAGE | NIF_TIP;
    m_notifyIconData.uCallbackMessage = m_uCallbackMessage;
    
    if (mIcon != nullptr)
    {
        m_notifyIconData.uFlags |= NIF_ICON;
        m_notifyIconData.hIcon = mIcon;
    }
    
    if (tip != nullptr)
    {
        wcsncpy_s(m_notifyIconData.szTip, tip, _countof(m_notifyIconData.szTip) - 1);
        m_notifyIconData.szTip[_countof(m_notifyIconData.szTip) - 1] = L'\0';
    }

    if (!Shell_NotifyIconW(NIM_ADD, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    m_created = true;
    return S_OK;
}

HRESULT __stdcall HutaoNativeNotifyIcon::Recreate(PCWSTR tip)
{
    if (!m_created)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    if (!Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (tip != nullptr)
    {
        wcsncpy_s(m_notifyIconData.szTip, tip, _countof(m_notifyIconData.szTip) - 1);
        m_notifyIconData.szTip[_countof(m_notifyIconData.szTip) - 1] = L'\0';
    }

    if (!Shell_NotifyIconW(NIM_ADD, &m_notifyIconData))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

HRESULT __stdcall HutaoNativeNotifyIcon::Destroy()
{
    if (!m_created)
    {
        return S_OK;
    }

    if (!Shell_NotifyIconW(NIM_DELETE, &m_notifyIconData))
    {
        // 即使删除失败，我们也继续清理
    }

    if (mWnd != nullptr)
    {
        DestroyWindow(mWnd);
        mWnd = nullptr;
    }

    if (mIcon != nullptr)
    {
        DestroyIcon(mIcon);
        mIcon = nullptr;
    }

    m_created = false;
    return S_OK;
}

HRESULT __stdcall HutaoNativeNotifyIcon::IsPromoted(BOOL* promoted)
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
