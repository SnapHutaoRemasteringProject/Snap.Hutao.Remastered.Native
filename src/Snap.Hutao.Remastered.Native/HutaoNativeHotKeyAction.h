#pragma once

#include "IHutaoNativeHotKeyAction_h.h"
#include "Types_h.h"
#include "CustomImplements.h"
#include "HutaoNativeHotKeyActionCallback.h"
#include <Windows.h>
#include <thread>
#include <atomic>

class HutaoNativeHotKeyAction : public hutao::CustomImplements<
    HutaoNativeHotKeyAction,
    IHutaoNativeHotKeyAction>
{
public:
	HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, nint userData);
    ~HutaoNativeHotKeyAction();

    // IHutaoNativeHotKeyAction methods
    virtual HRESULT STDMETHODCALLTYPE Update(int modifiers, uint vk) override;
    virtual HRESULT STDMETHODCALLTYPE GetIsEnabled(BOOL* isEnabled) override;
    virtual HRESULT STDMETHODCALLTYPE SetIsEnabled(BOOL isEnabled) override;

private:
    HutaoNativeHotKeyActionKind m_kind;
    HutaoNativeHotKeyActionCallback m_callback;
    nint m_userData;
    int m_modifiers;
    uint m_vk;
    bool m_enabled;
    int m_hotKeyId;
    HWND m_hWnd;
    std::atomic<bool> m_isRunning;
    std::thread m_actionThread;
    static UINT s_nextHotKeyId;

    // running state toggled by WM_HOTKEY
    std::atomic<bool> m_active;

    void UnregisterHotKey();
    void RegisterHotKey();

    // Window helpers
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    void UnregisterHotKey();
    void RegisterHotKey();
    
    // 动作执行函数
    void ExecuteAction();
    void StopAction();
    
    // 输入模拟函数
    void SimulateMouseClick();
    void SimulateKeyPress();
};
