#include "pch.h"
#include "HutaoNativeWindowSubclass.h"
#include <CommCtrl.h>
#include <shobjidl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Ole32.lib")

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
    , m_pTaskbarList(nullptr)
{
}

HutaoNativeWindowSubclass2::~HutaoNativeWindowSubclass2()
{
    if (m_pTaskbarList)
    {
        m_pTaskbarList->Release();
        m_pTaskbarList = nullptr;
    }
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
    
    if (!InitCommonControlsEx(&icex))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        return hr;
    }

    hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbarList));
    if (SUCCEEDED(hr) && m_pTaskbarList)
    {
        hr = m_pTaskbarList->HrInit();
        if (SUCCEEDED(hr))
        {
            m_initialized = true;
            return S_OK;
        }
        else
        {
            m_pTaskbarList->Release();
            m_pTaskbarList = nullptr;
            return hr;
        }
    }
    else
    {
        return hr;
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

    if (!m_pTaskbarList)
    {
        return E_UNEXPECTED;
    }

    // 获取调用线程的窗口句柄
    HWND hWnd = GetActiveWindow();
    if (hWnd == nullptr)
    {
        // 如果没有活动窗口，尝试获取控制台窗口
        hWnd = GetConsoleWindow();
        if (hWnd == nullptr)
        {
            return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
        }
    }

    // 根据flags设置进度状态
    TBPFLAG tbpFlags = TBPF_NORMAL;
    
    switch (flags)
    {
    case 0: // 正常进度
        tbpFlags = TBPF_NORMAL;
        break;
    case 1: // 暂停/错误状态
        tbpFlags = TBPF_ERROR;
        break;
    case 2: // 暂停状态
        tbpFlags = TBPF_PAUSED;
        break;
    case 3: // 不确定状态
        tbpFlags = TBPF_INDETERMINATE;
        break;
    case 4: // 无进度
        tbpFlags = TBPF_NOPROGRESS;
        break;
    default:
        tbpFlags = TBPF_NORMAL;
        break;
    }

    // 首先设置进度状态
    HRESULT hr = m_pTaskbarList->SetProgressState(hWnd, tbpFlags);
    if (FAILED(hr))
    {
        return hr;
    }

    // 如果状态不是无进度或不确定状态，则设置进度值
    if (tbpFlags != TBPF_NOPROGRESS && tbpFlags != TBPF_INDETERMINATE)
    {
        // 确保maximum不为0，避免除零错误
        if (maximum == 0)
        {
            maximum = 100; // 默认最大值
        }

        // 确保value不超过maximum
        if (value > maximum)
        {
            value = maximum;
        }

        hr = m_pTaskbarList->SetProgressValue(hWnd, value, maximum);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    return S_OK;
}
