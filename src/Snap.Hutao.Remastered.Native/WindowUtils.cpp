#include "WindowUtils.h"
#include <Windows.h>
#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

// 窗口切换函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSwitchToWindow(HWND hWnd)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    // 尝试将窗口带到前台
    if (!SetForegroundWindow(hWnd))
        return HRESULT_FROM_WIN32(GetLastError());

    // 激活窗口
    if (!SetActiveWindow(hWnd))
        return HRESULT_FROM_WIN32(GetLastError());

    // 将窗口设置为前台窗口
    if (!BringWindowToTop(hWnd))
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 添加分层窗口扩展样式
DLL_EXPORT HRESULT __stdcall WindowUtilitiesAddExtendedStyleLayered(HWND hWnd)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    
    if (SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 移除分层窗口扩展样式
DLL_EXPORT HRESULT __stdcall WindowUtilitiesRemoveExtendedStyleLayered(HWND hWnd)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    exStyle &= ~WS_EX_LAYERED;
    
    if (SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 设置分层窗口透明度
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetLayeredWindowTransparency(HWND hWnd, BYTE opacity)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    // 首先确保窗口有WS_EX_LAYERED样式
    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) == 0)
    {
        exStyle |= WS_EX_LAYERED;
        if (SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle) == 0)
            return HRESULT_FROM_WIN32(GetLastError());
    }

    // 设置透明度 (0=完全透明, 255=完全不透明)
    if (!SetLayeredWindowAttributes(hWnd, 0, opacity, LWA_ALPHA))
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 添加工具窗口扩展样式
DLL_EXPORT HRESULT __stdcall WindowUtilitiesAddExtendedStyleToolWindow(HWND hWnd)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOOLWINDOW;
    
    if (SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 移除重叠窗口样式
DLL_EXPORT HRESULT __stdcall WindowUtilitiesRemoveStyleOverlappedWindow(HWND hWnd)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    
    if (SetWindowLongPtr(hWnd, GWL_STYLE, style) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 获取窗口的光栅化缩放比例
DLL_EXPORT HRESULT __stdcall WindowUtilitiesGetRasterizationScaleForWindow(HWND hWnd, float* scale)
{
    if (!hWnd || !scale)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    // 获取窗口的DPI感知上下文
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (!hMonitor)
        return HRESULT_FROM_WIN32(GetLastError());

    // 获取DPI缩放比例
    UINT dpiX = 0, dpiY = 0;
    if (GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) != S_OK)
    {
        // 回退到系统DPI
        HDC hdc = GetDC(hWnd);
        dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(hWnd, hdc);
    }

    // 计算缩放比例 (96 DPI = 100% = 1.0)
    *scale = static_cast<float>(dpiX) / 96.0f;
    return S_OK;
}

// 设置窗口启用状态
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetWindowIsEnabled(HWND hWnd, BOOL isEnabled)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    if (!EnableWindow(hWnd, isEnabled))
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

// 设置窗口所有者
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetWindowOwner(HWND hWnd, HWND hWndOwner)
{
    if (!hWnd)
        return E_INVALIDARG;

    if (!IsWindow(hWnd))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    // 如果hWndOwner为NULL，则移除所有者
    if (hWndOwner && !IsWindow(hWndOwner))
        return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);

    if (!SetWindowLongPtr(hWnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hWndOwner)))
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}
