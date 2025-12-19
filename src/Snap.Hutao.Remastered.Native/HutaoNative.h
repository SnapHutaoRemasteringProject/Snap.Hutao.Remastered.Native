#pragma once

#include "IHutaoNative_h.h"
#include "IHutaoNativeLoopbackSupport_h.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNativeWindowNonRude_h.h"
#include "IHutaoNativeWindowSubclass_h.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "Types.h"
#include <winrt/base.h>
#include <hstring.h>
#include <Windows.h>

class HutaoNative : public winrt::implements<HutaoNative, IHutaoNative, winrt::non_agile>
{
public:
    HutaoNative() = default;
    ~HutaoNative() = default;

    virtual HRESULT STDMETHODCALLTYPE MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeRegistryNotification(HSTRING keyPath, IHutaoNativeRegistryNotification** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeWindowSubclass(INT64 hWnd, nint callback, INT64 userData, IHutaoNativeWindowSubclass** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeWindowNonRude(INT64 hWnd, IHutaoNativeWindowNonRude** ppv) override;
};
