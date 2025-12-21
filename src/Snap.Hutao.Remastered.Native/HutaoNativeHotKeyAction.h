#pragma once

#include "IHutaoNativeHotKeyAction_h.h"
#include "Types_h.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeHotKeyAction : public hutao::CustomImplements<
    HutaoNativeHotKeyAction,
    IHutaoNativeHotKeyAction>
{
public:
    HutaoNativeHotKeyAction();
	HutaoNativeHotKeyAction(HutaoNativeHotKeyActionKind kind, WNDPROC callback, LONG_PTR userData);
    ~HutaoNativeHotKeyAction();

    // IHutaoNativeHotKeyAction methods
    virtual HRESULT STDMETHODCALLTYPE Update(int modifiers, uint vk) override;
    virtual HRESULT STDMETHODCALLTYPE GetIsEnabled(BOOL* isEnabled) override;
    virtual HRESULT STDMETHODCALLTYPE SetIsEnabled(BOOL isEnabled) override;

private:
	WNDPROC m_callback;
    int m_modifiers;
    uint m_vk;
    bool m_enabled;
    int m_hotKeyId;
    HWND m_hWnd;
    static UINT s_nextHotKeyId;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static ATOM RegisterWindowClass();
    HWND CreateMessageWindow();
    void UnregisterHotKey();
    void RegisterHotKey();
};
