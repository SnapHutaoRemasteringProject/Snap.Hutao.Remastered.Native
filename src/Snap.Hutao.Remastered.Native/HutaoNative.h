#pragma once

#include "IHutaoNative_h.h"
#include "IHutaoNativeLoopbackSupport_h.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNativeWindowNonRude_h.h"
#include "IHutaoNativeWindowSubclass_h.h"
#include "IHutaoNativeDeviceCapabilities_h.h"
#include "IHutaoNativePhysicalDrive_h.h"
#include "IHutaoNativeLogicalDrive_h.h"
#include "IHutaoNativeInputLowLevelKeyboardSource_h.h"
#include "IHutaoNativeFileSystem_h.h"
#include "IHutaoNativeNotifyIcon_h.h"
#include "IHutaoNativeHotKeyAction_h.h"
#include "IHutaoNativeProcess_h.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "Types.h"
#include "CustomImplements.h"
#include <hstring.h>
#include <Windows.h>

class HutaoNative : public hutao::CustomImplements<
    HutaoNative, 
    IHutaoNative,
    IHutaoNativePrivate,
    IHutaoPrivate2,
    IHutaoNative2,
    IHutaoNative3,
    IHutaoNative4,
    IHutaoNative5,
    IHutaoNative6,
    IHutaoNative7>
{
public:
    HutaoNative() = default;
    ~HutaoNative() = default;

    // IHutaoNative methods
    virtual HRESULT STDMETHODCALLTYPE MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeRegistryNotification(PCWSTR keyPath, IHutaoNativeRegistryNotification** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeWindowSubclass(INT64 hWnd, nint callback, INT64 userData, IHutaoNativeWindowSubclass** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeWindowNonRude(INT64 hWnd, IHutaoNativeWindowNonRude** ppv) override;
    
    // IHutaoNative2 methods
    virtual HRESULT STDMETHODCALLTYPE MakeDeviceCapabilities(IHutaoNativeDeviceCapabilities** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakePhysicalDrive(IHutaoNativePhysicalDrive** ppv) override;
    virtual HRESULT STDMETHODCALLTYPE MakeLogicalDrive(IHutaoNativeLogicalDrive** ppv) override;
    
    // IHutaoNative3 methods
    virtual HRESULT STDMETHODCALLTYPE MakeInputLowLevelKeyboardSource(IHutaoNativeInputLowLevelKeyboardSource** ppv) override;
    
    // IHutaoNative4 methods
    virtual HRESULT STDMETHODCALLTYPE MakeFileSystem(IHutaoNativeFileSystem** ppv) override;
    
    // IHutaoNative5 methods
    virtual HRESULT STDMETHODCALLTYPE MakeNotifyIcon(PCWSTR iconPath, GUID* id, IHutaoNativeNotifyIcon** ppv) override;
    
    // IHutaoNative6 methods
    virtual HRESULT STDMETHODCALLTYPE MakeHotKeyAction(HutaoNativeHotKeyActionKind kind, nint callback, nint userData, IHutaoNativeHotKeyAction** ppv) override;
    
    // IHutaoNative7 methods
    virtual HRESULT STDMETHODCALLTYPE MakeProcess(HutaoNativeProcessStartInfo info, IHutaoNativeProcess** ppv) override;
    
    // IHutaoNativePrivate methods
    virtual HRESULT STDMETHODCALLTYPE IsCurrentWindowsVersionSupported(BOOL* isSupported) override;
    virtual HRESULT STDMETHODCALLTYPE GetWindowsVersion(HutaoPrivateWindowsVersion* pv) override;
    virtual HRESULT STDMETHODCALLTYPE ShowErrorMessage(PCWSTR title, PCWSTR message) override;
    
    // IHutaoPrivate2 methods
    virtual HRESULT STDMETHODCALLTYPE ExchangeGameUidForIdentifier1820(PCWSTR gameUid, byte* identifier) override;
};
