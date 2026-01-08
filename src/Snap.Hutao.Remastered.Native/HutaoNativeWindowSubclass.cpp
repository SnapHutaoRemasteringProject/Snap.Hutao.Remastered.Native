#include "HutaoNativeWindowSubclass.h"
#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")

static LRESULT CALLBACK SubclassWndProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData)
{
    HutaoNativeWindowSubclass* pThis = reinterpret_cast<HutaoNativeWindowSubclass*>(dwRefData);
    if (pThis == nullptr)
    {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (pThis->m_callback.has_value())
    {
        LRESULT lResult = 0;
        BOOL handled = pThis->m_callback.value()(hWnd, uMsg, wParam, lParam, pThis->m_userData, &lResult);
        if (handled)
        {
            return lResult;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HutaoNativeWindowSubclass::HutaoNativeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData)
    : mWnd(hWnd)
    , m_callback(callback)
    , m_userData(userData)
    , m_originalWndProc(nullptr)
    , m_attached(false)
{
}

HutaoNativeWindowSubclass::~HutaoNativeWindowSubclass()
{
    if (m_attached)
    {
        Detach();
    }
}

HRESULT __stdcall HutaoNativeWindowSubclass::Attach()
{
    if (m_attached)
    {
        return S_FALSE; // 已经附加
    }

    if (mWnd == nullptr || !IsWindow(mWnd))
    {
        return E_INVALIDARG;
    }

    // 使用SetWindowSubclass进行子类化
    BOOL result = SetWindowSubclass(
        mWnd,
        SubclassWndProc,
        1, // 子类ID
        reinterpret_cast<DWORD_PTR>(this));

    if (result)
    {
        m_attached = true;
        return S_OK;
    }
    else
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
}

HRESULT __stdcall HutaoNativeWindowSubclass::Detach()
{
    if (!m_attached)
    {
        return S_FALSE; // 已经分离
    }

    if (mWnd == nullptr || !IsWindow(mWnd))
    {
        return E_INVALIDARG;
    }

    // 移除子类化
    BOOL result = RemoveWindowSubclass(
        mWnd,
        SubclassWndProc,
        1); // 子类ID

    if (result)
    {
        m_attached = false;
        return S_OK;
    }
    else
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
}

// HutaoNativeWindowSubclass2 实现
HutaoNativeWindowSubclass2::HutaoNativeWindowSubclass2()
    : m_initialized(false)
{
}

HutaoNativeWindowSubclass2::~HutaoNativeWindowSubclass2()
{
}

HRESULT __stdcall HutaoNativeWindowSubclass2::InitializeTaskbarProgress()
{
    if (m_initialized)
    {
        return S_FALSE; // 已经初始化
    }

    // 初始化通用控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    
    if (InitCommonControlsEx(&icex))
    {
        m_initialized = true;
        return S_OK;
    }
    else
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
}

HRESULT __stdcall HutaoNativeWindowSubclass2::SetTaskbarProgress(
    UINT32 flags,
    ULONGLONG value,
    ULONGLONG maximum)
{
    if (!m_initialized)
    {
        // 如果没有初始化，尝试初始化
        HRESULT hr = InitializeTaskbarProgress();
        if (FAILED(hr))
        {
            return hr;
        }
    }

    // 这里需要实现任务栏进度条设置
    // 实际实现需要使用ITaskbarList3接口
    // 暂时返回E_NOTIMPL
    
    return E_NOTIMPL;
}
