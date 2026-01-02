#include "HutaoNativeHotKeyAction.h"
#include "Error.h"
#include <Windows.h>
#include <chrono>

const wchar_t* WINDOW_CLASS_NAME = L"HutaoNativeHotKeyActionWindowClass";
const UINT WM_HOTKEY_MESSAGE = WM_APP + 2;

UINT HutaoNativeHotKeyAction::s_nextHotKeyId = 0x4000; // 从0x4000开始，避免与系统热键冲突

HutaoNativeHotKeyAction::HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData)
    : m_callback(callback)
    , m_userData(userData)
    , m_kind(kind)
    , m_modifiers(0)
    , m_vk(0)
    , m_enabled(false)
    , m_hotKeyId(0)
    , m_hWnd(nullptr)
    , m_clicking(false)
    , m_clickIntervalMs(50) // default 50ms interval
    , m_keyPressing(false)
    , m_keyIntervalMs(100) // default 100ms interval for key press
    , m_active(false)
{
    m_hotKeyId = static_cast<int>(++s_nextHotKeyId);
}

HutaoNativeHotKeyAction::~HutaoNativeHotKeyAction()
{
    SetIsEnabled(FALSE);

    StopAutoClick();
    StopAutoKeyPress();

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
        reinterpret_cast<LPVOID>(this)
    );

    return hWnd;
}

LRESULT CALLBACK HutaoNativeHotKeyAction::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        return 0;
    }

    HutaoNativeHotKeyAction* pThis = reinterpret_cast<HutaoNativeHotKeyAction*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (pThis != nullptr)
    {
        if (message == WM_HOTKEY)
        {
            // Toggle active state on hotkey press
            bool newState = !pThis->m_active.load();
            pThis->m_active.store(newState);

            // Notify managed side about toggled state
            if (pThis->m_callback.value != nullptr)
            {
                BOOL isOn = newState ? TRUE : FALSE;
                pThis->m_callback.value(isOn, pThis->m_userData);
            }

            // Start/stop auto actions based on kind
            if (pThis->m_kind == HutaoNativeHotKeyActionKind::MouseClickRepeatForever)
            {
                if (pThis->m_active.load()) pThis->StartAutoClick(); else pThis->StopAutoClick();
            }
            else if (pThis->m_kind == HutaoNativeHotKeyActionKind::KeyPressRepeatForever)
            {
                if (pThis->m_active.load()) pThis->StartAutoKeyPress(); else pThis->StopAutoKeyPress();
            }

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

// Auto-click implementation
void HutaoNativeHotKeyAction::StartAutoClick()
{
    bool expected = false;
    if (!m_clicking.compare_exchange_strong(expected, true))
        return; // already clicking

    // Launch thread
    m_clickThread = std::thread([this]() {
        // Continue clicking until stopped
        while (m_clicking.load())
        {
            // Simulate left mouse down and up
            INPUT inputs[2];
            ZeroMemory(inputs, sizeof(inputs));

            inputs[0].type = INPUT_MOUSE;
            inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

            inputs[1].type = INPUT_MOUSE;
            inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

            SendInput(2, inputs, sizeof(INPUT));

            std::this_thread::sleep_for(std::chrono::milliseconds(m_clickIntervalMs));
        }
    });
}

void HutaoNativeHotKeyAction::StopAutoClick()
{
    bool expected = true;
    if (!m_clicking.compare_exchange_strong(expected, false))
        return; // not clicking

    if (m_clickThread.joinable())
    {
        m_clickThread.join();
    }
}

// Auto-keypress implementation
void HutaoNativeHotKeyAction::StartAutoKeyPress()
{
    bool expected = false;
    if (!m_keyPressing.compare_exchange_strong(expected, true))
        return; // already pressing

    m_keyThread = std::thread([this]() {
        // Repeatedly send configured vk or 'F'
        WORD vkToSend = (WORD)m_vk ? (WORD)m_vk : 0x46;
        INPUT inputs[2];
        ZeroMemory(inputs, sizeof(inputs));
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = vkToSend;
        inputs[0].ki.dwFlags = 0; // key down

        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = vkToSend;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP; // key up

        while (m_keyPressing.load())
        {
            SendInput(2, inputs, sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(m_keyIntervalMs));
        }
    });
}

void HutaoNativeHotKeyAction::StopAutoKeyPress()
{
    bool expected = true;
    if (!m_keyPressing.compare_exchange_strong(expected, false))
        return; // not pressing

    if (m_keyThread.joinable())
    {
        m_keyThread.join();
    }
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
    AssertNonNullAndReturn(isEnabled);

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
                HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
                ThrowForHR(hr, "CreateMessageWindow failed in SetIsEnabled");
                return hr;
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
