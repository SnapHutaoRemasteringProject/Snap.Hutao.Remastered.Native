#include "HutaoNativeWindowSubclass.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <winrt/base.h>

#pragma comment(lib, "Comctl32.lib")

// 窗口子类化回调函数
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

    // 调用用户提供的回调函数
    if (pThis->m_callback != nullptr)
    {
        // 这里需要调用用户回调，但回调签名可能不同
        // 暂时使用默认处理
    }

    // 调用原始窗口过程
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HutaoNativeWindowSubclass::HutaoNativeWindowSubclass(HWND hWnd, nint callback, LONG_PTR userData)
    : m_hWnd(hWnd)
    , m_callback(reinterpret_cast<WNDPROC>(callback))
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

HRESULT STDMETHODCALLTYPE HutaoNativeWindowSubclass::Attach()
{
    if (m_attached)
    {
        return S_FALSE; // 已经附加
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    // 使用SetWindowSubclass进行子类化
    BOOL result = SetWindowSubclass(
        m_hWnd,
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

HRESULT STDMETHODCALLTYPE HutaoNativeWindowSubclass::Detach()
{
    if (!m_attached)
    {
        return S_FALSE; // 已经分离
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    // 移除子类化
    BOOL result = RemoveWindowSubclass(
        m_hWnd,
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

HRESULT STDMETHODCALLTYPE HutaoNativeWindowSubclass2::InitializeTaskbarProgress()
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

HRESULT STDMETHODCALLTYPE HutaoNativeWindowSubclass2::SetTaskbarProgress(
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
