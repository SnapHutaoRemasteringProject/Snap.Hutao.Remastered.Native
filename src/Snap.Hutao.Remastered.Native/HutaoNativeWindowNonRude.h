#pragma once

#include "IHutaoNativeWindowNonRude_h.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeWindowNonRude :
    public hutao::CustomImplements<
    HutaoNativeWindowNonRude,
    IHutaoNativeWindowNonRude>
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
