#pragma once

#include "IHutaoNativeWindowSubclass_h.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <CommCtrl.h>

class HutaoNativeWindowSubclass :
    public hutao::CustomImplements<
    HutaoNativeWindowSubclass,
    IHutaoNativeWindowSubclass>
{
public:
    HutaoNativeWindowSubclass(HWND hWnd, nint callback, LONG_PTR userData);
    ~HutaoNativeWindowSubclass();

    virtual HRESULT STDMETHODCALLTYPE Attach() override;
    virtual HRESULT STDMETHODCALLTYPE Detach() override;
    WNDPROC m_callback;
private:
    HWND m_hWnd;
    LONG_PTR m_userData;
    WNDPROC m_originalWndProc;
    bool m_attached;
};

class HutaoNativeWindowSubclass2 :
    public hutao::CustomImplements<
    HutaoNativeWindowSubclass2,
    IHutaoNativeWindowSubclass2>
{
public:
    HutaoNativeWindowSubclass2();
    ~HutaoNativeWindowSubclass2();

    virtual HRESULT STDMETHODCALLTYPE InitializeTaskbarProgress() override;
    virtual HRESULT STDMETHODCALLTYPE SetTaskbarProgress(
        UINT32 flags,
        ULONGLONG value,
        ULONGLONG maximum) override;

private:
    bool m_initialized;
};
