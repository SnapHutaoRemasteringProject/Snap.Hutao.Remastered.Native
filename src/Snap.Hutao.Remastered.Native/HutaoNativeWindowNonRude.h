#pragma once

#include "IHutaoNativeWindowNonRude.h"
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

    virtual HRESULT __stdcall Attach() override;
    virtual HRESULT __stdcall Detach() override;

private:
    HWND m_hWnd;
    bool m_attached;
    LONG_PTR m_originalStyle;
};
