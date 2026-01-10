#pragma once

#include "IHutaoNativeHotKeyAction.h"
#include "Types.h"
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
	HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData);
    ~HutaoNativeHotKeyAction();

    // IHutaoNativeHotKeyAction methods
    virtual HRESULT __stdcall Update(int modifiers, uint vk) override;
    virtual HRESULT __stdcall GetIsEnabled(BOOL* isEnabled) override;
    virtual HRESULT __stdcall SetIsEnabled(BOOL isEnabled) override;

private:
    HutaoNativeHotKeyActionKind m_kind;
    HutaoNativeHotKeyActionCallback m_callback;
    GCHandle m_userData;
    int m_modifiers;
    uint m_vk;
    bool m_enabled;
    int motKeyId;
    HWND m_hWnd;
    std::atomic<bool> m_isRunning;
    std::thread m_actionThread;
    static UINT s_nextHotKeyId;

    // running state toggled by WM_HOTKEY
    std::atomic<bool> m_active;

    // Window helpers
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    void UnregisterHotKey();
    void RegisterHotKey();
    
    // 动作执行函数
    void ExecuteAction();
    void StopAction();
    
    // 输入模拟函数
    void SimulateMouseClick();
    void SimulateKeyPress();
};
