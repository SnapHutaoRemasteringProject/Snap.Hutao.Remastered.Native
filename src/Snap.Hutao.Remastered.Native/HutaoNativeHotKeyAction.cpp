#include "HutaoNativeHotKeyAction.h"
#include <Windows.h>

const wchar_t* WINDOW_CLASS_NAME = L"HutaoNativeHotKeyActionWindowClass";
const UINT WM_HOTKEY_MESSAGE = WM_APP + 2;

UINT HutaoNativeHotKeyAction::s_nextHotKeyId = 0x4000; // 从0x4000开始，避免与系统热键冲突

HutaoNativeHotKeyAction::HutaoNativeHotKeyAction()
    : m_callback(nullptr)
    , m_modifiers(0)
    , m_vk(0)
    , m_enabled(false)
    , m_hotKeyId(0)
    , m_hWnd(nullptr)
{
    m_hotKeyId = static_cast<int>(++s_nextHotKeyId);
}

HutaoNativeHotKeyAction::HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, WNDPROC callback, LONG_PTR userData)
{
	m_callback = callback;
    m_modifiers = 0;
    m_vk = 0;
    m_enabled = false;
    m_hotKeyId = static_cast<int>(++s_nextHotKeyId);
	m_hWnd = nullptr;
}

HutaoNativeHotKeyAction::~HutaoNativeHotKeyAction()
{
    SetIsEnabled(FALSE);
    if (m_hWnd != nullptr)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

ATOM HutaoNativeHotKeyAction::RegisterWindowClass()
{
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &HutaoNativeHotKeyAction::WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    
    return RegisterClassExW(&wc);
}

HWND HutaoNativeHotKeyAction::CreateMessageWindow()
{
    static ATOM classAtom = RegisterWindowClass();
    if (classAtom == 0)
    {
        return nullptr;
    }

    HWND hWnd = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        L"HutaoNativeHotKeyActionMessageWindow",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this // 将this指针传递给窗口
    );

    return hWnd;
}

LRESULT CALLBACK HutaoNativeHotKeyAction::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        return 0;
    }

    HutaoNativeHotKeyAction* pThis = reinterpret_cast<HutaoNativeHotKeyAction*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (pThis != nullptr)
    {
        if (message == WM_HOTKEY && pThis->m_callback != nullptr)
        {
            pThis->m_callback(hWnd, message, wParam, lParam);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

void HutaoNativeHotKeyAction::UnregisterHotKey()
{
    if (m_hWnd != nullptr && m_hotKeyId != 0)
    {
        ::UnregisterHotKey(m_hWnd, m_hotKeyId);
    }
}

void HutaoNativeHotKeyAction::RegisterHotKey()
{
    if (m_hWnd == nullptr || m_hotKeyId == 0 || m_vk == 0)
    {
        return;
    }

    ::RegisterHotKey(m_hWnd, m_hotKeyId, m_modifiers, m_vk);
}

HRESULT STDMETHODCALLTYPE HutaoNativeHotKeyAction::Update(int modifiers, uint vk)
{
    bool wasEnabled = m_enabled;
    
    if (wasEnabled)
    {
        UnregisterHotKey();
    }

    m_modifiers = modifiers;
    m_vk = vk;

    if (wasEnabled)
    {
        RegisterHotKey();
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeHotKeyAction::GetIsEnabled(BOOL* isEnabled)
{
    if (isEnabled == nullptr)
    {
        return E_POINTER;
    }

    *isEnabled = m_enabled ? TRUE : FALSE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeHotKeyAction::SetIsEnabled(BOOL isEnabled)
{
    bool newEnabled = (isEnabled != FALSE);

    if (newEnabled == m_enabled)
    {
        return S_OK; // 状态没有变化
    }

    if (newEnabled)
    {
        // 启用热键
        if (m_hWnd == nullptr)
        {
            m_hWnd = CreateMessageWindow();
            if (m_hWnd == nullptr)
            {
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }

        if (m_vk != 0)
        {
            RegisterHotKey();
        }
    }
    else
    {
        // 禁用热键
        UnregisterHotKey();
    }

    m_enabled = newEnabled;
    return S_OK;
}
