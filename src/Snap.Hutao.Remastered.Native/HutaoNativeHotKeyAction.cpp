#include "HutaoNativeHotKeyAction.h"
#include "HutaoNativeHotKeyActionCallback.h"
#include "Error.h"
#include <Windows.h>
#include <chrono>
#include <thread>

const wchar_t* WINDOW_CLASS_NAME = L"HutaoNativeHotKeyActionWindowClass";
const UINT WM_HOTKEY_MESSAGE = WM_APP + 2;

UINT HutaoNativeHotKeyAction::s_nextHotKeyId = 0x4000; // 从0x4000开始，避免与系统热键冲突

HutaoNativeHotKeyAction::HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, nint userData)
    : m_kind(kind)
    , m_callback(callback)
    , m_userData(userData)
    , m_modifiers(0)
    , m_vk(0)
    , m_enabled(false)
    , m_hotKeyId(0)
    , m_hWnd(nullptr)
    , m_isRunning(false)
{
    m_hotKeyId = static_cast<int>(++s_nextHotKeyId);
}

HutaoNativeHotKeyAction::~HutaoNativeHotKeyAction()
{
    SetIsEnabled(FALSE);
    StopAction();
    
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
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        return 0;
    }

    HutaoNativeHotKeyAction* pThis = reinterpret_cast<HutaoNativeHotKeyAction*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (pThis != nullptr)
    {
        if (message == WM_HOTKEY)
        {
            // 热键被触发，切换动作状态
            bool wasRunning = pThis->m_isRunning.load();
            
            if (wasRunning)
            {
                pThis->StopAction();
            }
            else
            {
                pThis->ExecuteAction();
            }
            
            // 调用回调函数通知状态变化
            if (pThis->m_callback.value != nullptr)
            {
                pThis->m_callback.value(!wasRunning ? TRUE : FALSE, pThis->m_userData);
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

void HutaoNativeHotKeyAction::ExecuteAction()
{
    if (m_isRunning.load())
    {
        return; // 已经在运行
    }
    
    m_isRunning.store(true);
    
    // 启动新线程执行动作
    m_actionThread = std::thread([this]() {
        while (m_isRunning.load())
        {
            if (m_kind == MouseClickRepeatForever)
            {
                SimulateMouseClick();
            }
            else if (m_kind == KeyPressRepeatForever)
            {
                SimulateKeyPress();
            }
            
            // 延迟一段时间（例如50ms，即每秒20次）
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
}

void HutaoNativeHotKeyAction::StopAction()
{
    m_isRunning.store(false);
    
    if (m_actionThread.joinable())
    {
        m_actionThread.join();
    }
}

void HutaoNativeHotKeyAction::SimulateMouseClick()
{
    // 模拟鼠标左键点击
    INPUT inputs[2] = {0};
    
    // 按下左键
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    // 释放左键
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    SendInput(2, inputs, sizeof(INPUT));
}

void HutaoNativeHotKeyAction::SimulateKeyPress()
{
    // 模拟F键按下（VK_F为0x46）
    INPUT inputs[2] = {0};
    
    // 按下F键
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = 0x46; // VK_F
    inputs[0].ki.dwFlags = 0;
    
    // 释放F键
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0x46; // VK_F
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(2, inputs, sizeof(INPUT));
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
        StopAction();
    }

    m_enabled = newEnabled;
    return S_OK;
}
