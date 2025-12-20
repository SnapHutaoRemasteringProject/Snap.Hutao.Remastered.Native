#include "HutaoNativeInputLowLevelKeyboardSource.h"
#include <Windows.h>

// 全局变量来存储钩子句柄和回调函数
static HHOOK g_keyboardHook = nullptr;
static void* g_callback = nullptr;

// 键盘钩子过程函数
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && g_callback != nullptr)
    {
        // 调用回调函数
        // 回调函数签名：BOOL (*callback)(uint, KBDLLHOOKSTRUCT*)
        typedef BOOL (CALLBACK* KeyboardCallback)(uint, KBDLLHOOKSTRUCT*);
        KeyboardCallback callback = reinterpret_cast<KeyboardCallback>(g_callback);
        
        KBDLLHOOKSTRUCT* pKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        uint message = static_cast<uint>(wParam);
        
        // 调用回调，如果返回TRUE表示已处理，停止传递
        if (callback && callback(message, pKeyboardStruct))
        {
            return 1; // 阻止消息传递
        }
    }
    
    // 继续传递消息
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

HRESULT STDMETHODCALLTYPE HutaoNativeInputLowLevelKeyboardSource::Attach(nint callback) noexcept
{
    if (callback == 0)
    {
        return E_INVALIDARG;
    }
    
    // 如果已经安装了钩子，先移除
    if (g_keyboardHook != nullptr)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    
    // 保存回调函数
    g_callback = reinterpret_cast<void*>(callback);
    
    // 安装低级键盘钩子
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

HRESULT STDMETHODCALLTYPE HutaoNativeInputLowLevelKeyboardSource::Detach(nint callback) noexcept
{
    // 验证回调函数是否匹配
    if (reinterpret_cast<void*>(callback) != g_callback)
    {
        return E_INVALIDARG;
    }
    
    if (g_keyboardHook != nullptr)
    {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
        g_callback = nullptr;
        return S_OK;
    }
    
    // 如果没有钩子，返回成功（已经处于分离状态）
    return S_OK;
}
