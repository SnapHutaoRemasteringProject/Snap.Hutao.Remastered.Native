#pragma once

#include "IHutaoNative.h"
#include "IHutaoNativeLoopbackSupport.h"
#include "IHutaoNativeRegistryNotification.h"
#include "IHutaoNativeWindowNonRude.h"
#include "IHutaoNativeWindowSubclass.h"
#include "IHutaoNativeDeviceCapabilities.h"
#include "IHutaoNativePhysicalDrive.h"
#include "IHutaoNativeLogicalDrive.h"
#include "IHutaoNativeInputLowLevelKeyboardSource.h"
#include "IHutaoNativeFileSystem.h"
#include "IHutaoNativeNotifyIcon.h"
#include "IHutaoNativeHotKeyAction.h"
#include "IHutaoNativeProcess.h"
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
    virtual HRESULT __stdcall MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv) override;
    virtual HRESULT __stdcall MakeRegistryNotification(PCWSTR keyPath, IHutaoNativeRegistryNotification** ppv) override;
    virtual HRESULT __stdcall MakeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData, IHutaoNativeWindowSubclass** ppv) override;
    virtual HRESULT __stdcall MakeWindowNonRude(HWND hWnd, IHutaoNativeWindowNonRude** ppv) override;
    
    // IHutaoNative2 methods
    virtual HRESULT __stdcall MakeDeviceCapabilities(IHutaoNativeDeviceCapabilities** ppv) override;
    virtual HRESULT __stdcall MakePhysicalDrive(IHutaoNativePhysicalDrive** ppv) override;
    virtual HRESULT __stdcall MakeLogicalDrive(IHutaoNativeLogicalDrive** ppv) override;
    
    // IHutaoNative3 methods
    virtual HRESULT __stdcall MakeInputLowLevelKeyboardSource(IHutaoNativeInputLowLevelKeyboardSource** ppv) override;
    
    // IHutaoNative4 methods
    virtual HRESULT __stdcall MakeFileSystem(IHutaoNativeFileSystem** ppv) override;
    
    // IHutaoNative5 methods
    virtual HRESULT __stdcall MakeNotifyIcon(PCWSTR iconPath, GUID* id, IHutaoNativeNotifyIcon** ppv) override;
    
    // IHutaoNative6 methods
    virtual HRESULT __stdcall MakeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData, IHutaoNativeHotKeyAction** ppv) override;
    
    // IHutaoNative7 methods
    virtual HRESULT __stdcall MakeProcess(HutaoNativeProcessStartInfo info, IHutaoNativeProcess** ppv) override;
    
    // IHutaoNativePrivate methods
    virtual HRESULT __stdcall IsCurrentWindowsVersionSupported(BOOL* isSupported) override;
    virtual HRESULT __stdcall GetWindowsVersion(HutaoPrivateWindowsVersion* pv) override;
    virtual HRESULT __stdcall ShowErrorMessage(PCWSTR title, PCWSTR message) override;
    
    // IHutaoPrivate2 methods
    virtual HRESULT __stdcall ExchangeGameUidForIdentifier1820(PCWSTR gameUid, byte* identifier) override;
};
