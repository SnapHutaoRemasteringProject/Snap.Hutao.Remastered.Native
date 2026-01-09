#pragma once

#include "IHutaoNativeWindowSubclass.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <shobjidl.h>

#define WINDOW_SUBCLASS_ID 0x65

LRESULT CALLBACK SubclassWndProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData);

class HutaoNativeWindowSubclass :
    public hutao::CustomImplements<
    HutaoNativeWindowSubclass,
    IHutaoNativeWindowSubclass>
{
public:
    HutaoNativeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData);
    ~HutaoNativeWindowSubclass();

    virtual HRESULT __stdcall Attach() override;
    virtual HRESULT __stdcall Detach() override;
    
private:
    HWND m_hWnd;
    HutaoNativeWindowSubclassCallback m_callback;
    GCHandle m_userData;
    WNDPROC m_originalWndProc;
    bool m_attached;

    friend LRESULT CALLBACK SubclassWndProc(
        HWND hWnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        UINT_PTR uIdSubclass,
		DWORD_PTR dwRefData);
};

class HutaoNativeWindowSubclass2 :
    public hutao::CustomImplements<
    HutaoNativeWindowSubclass2,
    IHutaoNativeWindowSubclass2>
{
public:
    HutaoNativeWindowSubclass2();
    ~HutaoNativeWindowSubclass2();

    virtual HRESULT __stdcall InitializeTaskbarProgress() override;
    virtual HRESULT __stdcall SetTaskbarProgress(
        UINT32 flags,
        ULONGLONG value,
        ULONGLONG maximum) override;

private:
    bool m_initialized;
    ITaskbarList3* m_pTaskbarList;
};
