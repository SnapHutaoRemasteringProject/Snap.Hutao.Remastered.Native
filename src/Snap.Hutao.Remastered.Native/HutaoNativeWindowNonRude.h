#pragma once

#include "IHutaoNativeWindowNonRude_h.h"
#include <Windows.h>
#include <winrt/base.h>

class HutaoNativeWindowNonRude :
    public winrt::implements<
    HutaoNativeWindowNonRude,
    IHutaoNativeWindowNonRude,
    winrt::non_agile>
{
public:
    HutaoNativeWindowNonRude(HWND hWnd);
    ~HutaoNativeWindowNonRude();

    virtual HRESULT STDMETHODCALLTYPE Attach() override;
    virtual HRESULT STDMETHODCALLTYPE Detach() override;

private:
    HWND m_hWnd;
    bool m_attached;
    LONG_PTR m_originalStyle;
};
