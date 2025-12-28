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
        return S_FALSE;
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    SetPropW(m_hWnd, L"NonRudeHWND", (HANDLE)TRUE);

    m_attached = true;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeWindowNonRude::Detach()
{
    if (!m_attached)
    {
        return S_FALSE;
    }

    if (m_hWnd == nullptr || !IsWindow(m_hWnd))
    {
        return E_INVALIDARG;
    }

    SetPropW(m_hWnd, L"NonRudeHWND", (HANDLE)FALSE);

    m_attached = false;
    return S_OK;
}
