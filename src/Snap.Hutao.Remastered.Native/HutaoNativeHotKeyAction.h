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
	HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData);
    ~HutaoNativeHotKeyAction();

    // IHutaoNativeHotKeyAction methods
    virtual HRESULT STDMETHODCALLTYPE Update(int modifiers, uint vk) override;
    virtual HRESULT STDMETHODCALLTYPE GetIsEnabled(BOOL* isEnabled) override;
    virtual HRESULT STDMETHODCALLTYPE SetIsEnabled(BOOL isEnabled) override;

private:
	HutaoNativeHotKeyActionCallback m_callback;
	GCHandle m_userData;
	HutaoNativeHotKeyActionKind m_kind;
    int m_modifiers;
    uint m_vk;
    bool m_enabled;
    int m_hotKeyId;
    HWND m_hWnd;
    static UINT s_nextHotKeyId;

    // running state toggled by WM_HOTKEY
    std::atomic<bool> m_active;

    void UnregisterHotKey();
    void RegisterHotKey();

    // Window helpers
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Auto-click support
    std::thread m_clickThread;
    std::atomic<bool> m_clicking;
    int m_clickIntervalMs;

    void StartAutoClick();
    void StopAutoClick();

    // Auto-keypress support (for KeyPressRepeatForever)
    std::thread m_keyThread;
    std::atomic<bool> m_keyPressing;
    int m_keyIntervalMs;

    void StartAutoKeyPress();
    void StopAutoKeyPress();
};
