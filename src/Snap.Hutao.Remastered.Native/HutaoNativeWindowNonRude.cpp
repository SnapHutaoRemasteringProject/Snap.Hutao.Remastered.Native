#include "HutaoNativeWindowNonRude.h"
#include <Windows.h>

HutaoNativeWindowNonRude::HutaoNativeWindowNonRude(HWND hWnd)
    : m_hWnd(hWnd)
    , m_attached(false)
    , m_originalStyle(0)
{
}

HutaoNativeWindowNonRude::~HutaoNativeWindowNonRude()
{
    if (m_attached)
    {
        Detach();
    }
}

HRESULT STDMETHODCALLTYPE HutaoNativeWindowNonRude::Attach()
{
    if (m_attached)
    {
        return S_FALSE; // 已经附加
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    // 获取当前扩展样式
    LONG_PTR currentStyle = GetWindowLongPtrW(m_hWnd, GWL_EXSTYLE);
    if (currentStyle == 0)
    {
        // GetWindowLongPtr失败
        DWORD error = GetLastError();
        if (error != 0)
        {
            return HRESULT_FROM_WIN32(error);
        }
    }

    // 保存原始样式
    m_originalStyle = currentStyle;

    // 设置non-rude样式：
    // 1. 保留WS_EX_APPWINDOW，以允许窗口出现在任务栏
    // 2. 不添加WS_EX_TOOLWINDOW，因为工具窗口不会出现在任务栏
    // 3. 添加WS_EX_NOACTIVATE（防止窗口激活）
    LONG_PTR newStyle = currentStyle;
    
    // 添加WS_EX_NOACTIVATE（防止窗口激活）
    newStyle |= WS_EX_NOACTIVATE;

    // 应用新样式
    if (SetWindowLongPtrW(m_hWnd, GWL_EXSTYLE, newStyle) == 0)
    {
        DWORD error = GetLastError();
        if (error != 0)
        {
            return HRESULT_FROM_WIN32(error);
        }
    }

    // 强制重绘窗口以应用样式更改
    SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    m_attached = true;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeWindowNonRude::Detach()
{
    if (!m_attached)
    {
        return S_FALSE; // 已经分离
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    // 恢复原始样式
    if (SetWindowLongPtrW(m_hWnd, GWL_EXSTYLE, m_originalStyle) == 0)
    {
        DWORD error = GetLastError();
        if (error != 0)
        {
            return HRESULT_FROM_WIN32(error);
        }
    }

    // 强制重绘窗口以应用样式更改
    SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    m_attached = false;
    return S_OK;
}
