#include "pch.h"
#include "HutaoNativeWindowNonRude.h"

HutaoNativeWindowNonRude::HutaoNativeWindowNonRude(HWND hWnd)
    : mWnd(hWnd)
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

HRESULT __stdcall HutaoNativeWindowNonRude::Attach()
{
    if (m_attached)
    {
        return S_FALSE;
    }

    if (mWnd == nullptr || !IsWindow(mWnd))
    {
        return E_INVALIDARG;
    }

    SetPropW(mWnd, L"NonRudeHWND", (HANDLE)TRUE);

    m_attached = true;
    return S_OK;
}

HRESULT __stdcall HutaoNativeWindowNonRude::Detach()
{
    if (!m_attached)
    {
        return S_FALSE;
    }

    if (mWnd == nullptr || !IsWindow(mWnd))
    {
        return E_INVALIDARG;
    }

    SetPropW(mWnd, L"NonRudeHWND", (HANDLE)FALSE);

    m_attached = false;
    return S_OK;
}
