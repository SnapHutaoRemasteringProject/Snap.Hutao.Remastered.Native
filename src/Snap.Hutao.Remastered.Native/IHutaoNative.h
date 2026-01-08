#pragma once

#include <Windows.h>
#include <unknwn.h>
#include "types.h"
#include "IHutaoString.h"
#include "IHutaoNativeLoopbackSupport.h"
#include "IHutaoNativeRegistryNotification.h"
#include "IHutaoNativeWindowSubclass.h"
#include "IHutaoNativeWindowNonRude.h"
#include "IHutaoNativeDeviceCapabilities.h"
#include "IHutaoNativePhysicalDrive.h"
#include "IHutaoNativeLogicalDrive.h"
#include "IHutaoNativeInputLowLevelKeyboardSource.h"
#include "IHutaoNativeFileSystem.h"
#include "IHutaoNativeNotifyIcon.h"
#include "IHutaoNativeHotKeyAction.h"
#include "IHutaoNativeProcess.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "HutaoNativeHotKeyActionCallback.h"

// {1A6980D9-EB36-4E3E-86E7-3665C57C6E8D}
class __declspec(uuid("1A6980D9-EB36-4E3E-86E7-3665C57C6E8D")) IHutaoNativePrivate : public IUnknown
{
public:
    virtual HRESULT __stdcall IsCurrentWindowsVersionSupported(BOOL* isSupported) = 0;
    virtual HRESULT __stdcall GetWindowsVersion(HutaoPrivateWindowsVersion* pv) = 0;
    virtual HRESULT __stdcall ShowErrorMessage(PCWSTR title, PCWSTR message) = 0;
};

// {4E5D37CF-5F38-4FF2-9059-DF39CA696365}
class __declspec(uuid("4E5D37CF-5F38-4FF2-9059-DF39CA696365")) IHutaoPrivate2 : public IUnknown
{
public:
    virtual HRESULT __stdcall ExchangeGameUidForIdentifier1820(PCWSTR gameUid, byte* identifier) = 0;
};

// {D00F73FF-A1C7-4091-8CB6-D90991DD40CB}
class __declspec(uuid("D00F73FF-A1C7-4091-8CB6-D90991DD40CB")) IHutaoNative : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv) = 0;
    virtual HRESULT __stdcall MakeRegistryNotification(PCWSTR keyPath, IHutaoNativeRegistryNotification** ppv) = 0;
    virtual HRESULT __stdcall MakeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData, IHutaoNativeWindowSubclass** ppv) = 0;
    virtual HRESULT __stdcall MakeWindowNonRude(HWND hWnd, IHutaoNativeWindowNonRude** ppv) = 0;
};

// {338487EE-9592-4171-89DD-1E6B9EDB2C8E}
class __declspec(uuid("338487EE-9592-4171-89DD-1E6B9EDB2C8E")) IHutaoNative2 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeDeviceCapabilities(IHutaoNativeDeviceCapabilities** ppv) = 0;
    virtual HRESULT __stdcall MakePhysicalDrive(IHutaoNativePhysicalDrive** ppv) = 0;
    virtual HRESULT __stdcall MakeLogicalDrive(IHutaoNativeLogicalDrive** ppv) = 0;
};

// {135FACE1-3184-4D12-B4D0-21FFB6A88D25}
class __declspec(uuid("135FACE1-3184-4D12-B4D0-21FFB6A88D25")) IHutaoNative3 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeInputLowLevelKeyboardSource(IHutaoNativeInputLowLevelKeyboardSource** ppv) = 0;
};

// {27942FBE-322F-4157-9B8C-A38FDB827B05}
class __declspec(uuid("27942FBE-322F-4157-9B8C-A38FDB827B05")) IHutaoNative4 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeFileSystem(IHutaoNativeFileSystem** ppv) = 0;
};

// {7B4D20F1-4DAD-492E-8485-B4701A2ED19B}
class __declspec(uuid("7B4D20F1-4DAD-492E-8485-B4701A2ED19B")) IHutaoNative5 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeNotifyIcon(PCWSTR iconPath, GUID* id, IHutaoNativeNotifyIcon** ppv) = 0;
};

// {B68CABFA-C55A-49FA-8C51-21615C594E2B}
class __declspec(uuid("B68CABFA-C55A-49FA-8C51-21615C594E2B")) IHutaoNative6 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData, IHutaoNativeHotKeyAction** ppv) = 0;
};

// {B7A49A20-D9E2-43FD-9637-E80190443ABE}
class __declspec(uuid("B7A49A20-D9E2-43FD-9637-E80190443ABE")) IHutaoNative7 : public IUnknown
{
public:
    virtual HRESULT __stdcall MakeProcess(HutaoNativeProcessStartInfo info, IHutaoNativeProcess** ppv) = 0;
};
