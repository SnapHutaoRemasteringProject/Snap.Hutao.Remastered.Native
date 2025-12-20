#include "HutaoNative.h"
#include "HutaoNativeRegistryNotification.h"
#include "IHutaoNativeLoopbackSupport_h.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNativeWindowNonRude_h.h"
#include "IHutaoNativeWindowSubclass_h.h"
#include "HutaoNativeLoopbackSupport.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "types.h"
#include <winrt/base.h>
#include <Windows.h>
#include <hstring.h>

using namespace winrt;

HRESULT STDMETHODCALLTYPE HutaoNative::MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    com_ptr<IHutaoNativeLoopbackSupport> support = make_self<HutaoNativeLoopbackSupport>();
    *ppv = detach_abi(support);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeRegistryNotification(HSTRING keyPath, IHutaoNativeRegistryNotification** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    com_ptr<IHutaoNativeRegistryNotification> notify = make_self<HutaoNativeRegistryNotification>();
    *ppv = detach_abi(notify);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeWindowSubclass(INT64 hWnd, nint callback, INT64 userData, IHutaoNativeWindowSubclass** ppv)
{
    if (ppv) *ppv = nullptr;
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeWindowNonRude(INT64 hWnd, IHutaoNativeWindowNonRude** ppv)
{
    if (ppv) *ppv = nullptr;
    return E_NOTIMPL;
}
