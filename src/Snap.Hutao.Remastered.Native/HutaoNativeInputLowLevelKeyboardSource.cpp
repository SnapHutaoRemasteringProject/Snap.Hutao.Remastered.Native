#include "pch.h"
#include "HutaoNativeInputLowLevelKeyboardSource.h"

static HHOOK g_keyboardHook = nullptr;
static void* g_callback = nullptr;

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && g_callback != nullptr)
    {
        typedef BOOL (CALLBACK* KeyboardCallback)(uint, KBDLLHOOKSTRUCT*);
        KeyboardCallback callback = reinterpret_cast<KeyboardCallback>(g_callback);
        
        KBDLLHOOKSTRUCT* pKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        uint message = static_cast<uint>(wParam);
        
        if (callback && callback(message, pKeyboardStruct))
        {
            return 1;
        }
    }
    
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

HRESULT __stdcall HutaoNativeInputLowLevelKeyboardSource::Attach(intptr_t callback) noexcept
{
    if (callback == 0)
    {
        return E_INVALIDARG;
    }
    
    if (g_keyboardHook != nullptr)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    
    g_callback = reinterpret_cast<void*>(callback);
    
    g_keyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandleW(nullptr),
        0
    );
    
    if (g_keyboardHook == nullptr)
    {
        DWORD error = GetLastError();
        g_callback = nullptr;
        return HRESULT_FROM_WIN32(error);
    }
    
    return S_OK;
}

HRESULT __stdcall HutaoNativeInputLowLevelKeyboardSource::Detach(intptr_t callback) noexcept
{
    if (g_keyboardHook != nullptr)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
        g_callback = nullptr;
        return S_OK;
    }
    
    return S_OK;
}
