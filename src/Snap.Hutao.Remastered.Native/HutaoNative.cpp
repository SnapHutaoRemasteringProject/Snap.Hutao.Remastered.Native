#include "HutaoNative.h"
#include "HutaoNativeRegistryNotification.h"
#include "IHutaoNativeLoopbackSupport_h.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNativeWindowNonRude_h.h"
#include "IHutaoNativeWindowSubclass_h.h"
#include "HutaoNativeLoopbackSupport.h"
#include "HutaoNativeWindowSubclass.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "HutaoNativeDeviceCapabilities.h"
#include "HutaoNativePhysicalDrive.h"
#include "HutaoNativeLogicalDrive.h"
#include "HutaoNativeInputLowLevelKeyboardSource.h"
#include "HutaoNativeFileSystem.h"
#include "HutaoNativeNotifyIcon.h"
#include "HutaoNativeHotKeyAction.h"
#include "HutaoNativeProcess.h"
#include "HutaoNativeWindowNonRude.h"
#include "types.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <hstring.h>
#include <winternl.h>

HRESULT STDMETHODCALLTYPE HutaoNative::MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeLoopbackSupport> support = hutao::make_com_ptr<HutaoNativeLoopbackSupport>();
    *ppv = support.detach();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeRegistryNotification(PCWSTR keyPath, IHutaoNativeRegistryNotification** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeRegistryNotification> notify = hutao::make_com_ptr<HutaoNativeRegistryNotification>();
    *ppv = notify.detach();

    // Mark unused parameter to avoid warning
    (void)keyPath;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeWindowSubclass(INT64 hWnd, nint callback, INT64 userData, IHutaoNativeWindowSubclass** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    // Convert INT64 to HWND
    HWND hwnd = reinterpret_cast<HWND>(hWnd);
    
    // Convert nint to LONG_PTR for constructor
    LONG_PTR callbackPtr = static_cast<LONG_PTR>(callback);
    
    // Convert INT64 to LONG_PTR
    LONG_PTR userDataPtr = static_cast<LONG_PTR>(userData);

    hutao::com_ptr<IHutaoNativeWindowSubclass> subclass = hutao::make_com_ptr<HutaoNativeWindowSubclass>(hwnd, callbackPtr, userDataPtr);
    *ppv = subclass.detach();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeWindowNonRude(INT64 hWnd, IHutaoNativeWindowNonRude** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    // Convert INT64 to HWND
    HWND hwnd = reinterpret_cast<HWND>(hWnd);

    hutao::com_ptr<IHutaoNativeWindowNonRude> nonRude = hutao::make_com_ptr<HutaoNativeWindowNonRude>(hwnd);
    *ppv = nonRude.detach();

    return S_OK;
}

// IHutaoNative2 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeDeviceCapabilities(IHutaoNativeDeviceCapabilities** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeDeviceCapabilities> capabilities = hutao::make_com_ptr<HutaoNativeDeviceCapabilities>();
    *ppv = capabilities.detach();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakePhysicalDrive(IHutaoNativePhysicalDrive** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativePhysicalDrive> physicalDrive = hutao::make_com_ptr<HutaoNativePhysicalDrive>();
    *ppv = physicalDrive.detach();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::MakeLogicalDrive(IHutaoNativeLogicalDrive** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeLogicalDrive> logicalDrive = hutao::make_com_ptr<HutaoNativeLogicalDrive>();
    *ppv = logicalDrive.detach();

    return S_OK;
}

// IHutaoNative3 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeInputLowLevelKeyboardSource(IHutaoNativeInputLowLevelKeyboardSource** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeInputLowLevelKeyboardSource> source = hutao::make_com_ptr<HutaoNativeInputLowLevelKeyboardSource>();
    *ppv = source.detach();

    return S_OK;
}

// IHutaoNative4 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeFileSystem(IHutaoNativeFileSystem** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeFileSystem> fileSystem = hutao::make_com_ptr<HutaoNativeFileSystem>();
    *ppv = fileSystem.detach();

    return S_OK;
}

// IHutaoNative5 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeNotifyIcon(PCWSTR iconPath, GUID* id, IHutaoNativeNotifyIcon** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeNotifyIcon> notifyIcon = hutao::make_com_ptr<HutaoNativeNotifyIcon>();
    *ppv = notifyIcon.detach();

    // Mark unused parameters to avoid warning
    (void)iconPath;
    (void)id;

    return S_OK;
}

// IHutaoNative6 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeHotKeyAction(HutaoNativeHotKeyActionKind kind, nint callback, nint userData, IHutaoNativeHotKeyAction** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    // Convert nint to appropriate types
    WNDPROC wndProc = reinterpret_cast<WNDPROC>(callback);
    LONG_PTR userDataPtr = static_cast<LONG_PTR>(userData);

    hutao::com_ptr<IHutaoNativeHotKeyAction> hotKeyAction = hutao::make_com_ptr<HutaoNativeHotKeyAction>(kind, wndProc, userDataPtr);
    *ppv = hotKeyAction.detach();

    return S_OK;
}

// IHutaoNative7 methods
HRESULT STDMETHODCALLTYPE HutaoNative::MakeProcess(HutaoNativeProcessStartInfo info, IHutaoNativeProcess** ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }

    hutao::com_ptr<IHutaoNativeProcess> process = hutao::make_com_ptr<HutaoNativeProcess>(info);
    *ppv = process.detach();

    // Mark unused parameter to avoid warning
    (void)info;

    return S_OK;
}

// IHutaoNativePrivate methods
HRESULT STDMETHODCALLTYPE HutaoNative::IsCurrentWindowsVersionSupported(BOOL* isSupported)
{
    if (isSupported == nullptr)
    {
        return E_POINTER;
    }

    // Simple implementation: always return TRUE (supported)
    // Actual implementation should check Windows version
    *isSupported = TRUE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::GetWindowsVersion(HutaoPrivateWindowsVersion* pv)
{
    if (pv == nullptr)
    {
        return E_POINTER;
    }

    // Use RtlGetVersion to get accurate Windows version information
    typedef LONG (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    RtlGetVersionPtr pRtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(
        GetProcAddress(hNtdll, "RtlGetVersion"));
    
    if (pRtlGetVersion == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    RTL_OSVERSIONINFOW versionInfo = { 0 };
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    
    LONG status = pRtlGetVersion(&versionInfo);
    if (status != 0) // STATUS_SUCCESS is 0
    {
        return HRESULT_FROM_NT(status);
    }

    // Fill the structure
    pv->major = versionInfo.dwMajorVersion;
    pv->minor = versionInfo.dwMinorVersion;
    pv->build = versionInfo.dwBuildNumber;
    pv->revision = 0; // RTL_OSVERSIONINFOW doesn't have revision field, set to 0

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNative::ShowErrorMessage(PCWSTR title, PCWSTR message)
{
    if (title == nullptr || message == nullptr)
    {
        return E_INVALIDARG;
    }

    // Show message box
    int result = MessageBoxW(
        nullptr,           // No parent window
        message,           // Message text
        title,             // Title
        MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST
    );

    // MessageBoxW returns IDOK (1) when user clicks OK button
    // We always return S_OK because message box was shown successfully
    return S_OK;
}

// IHutaoPrivate2 methods
HRESULT STDMETHODCALLTYPE HutaoNative::ExchangeGameUidForIdentifier1820(PCWSTR gameUid, byte* identifier)
{
    if (gameUid == nullptr || identifier == nullptr)
    {
        return E_INVALIDARG;
    }

    // Calculate string length (excluding null terminator)
    size_t length = 0;
    while (gameUid[length] != L'\0')
    {
        length++;
    }

    // Copy string to output buffer as UTF-16 byte sequence
    // Each character is 2 bytes (wchar_t)
    size_t byteCount = length * sizeof(wchar_t);
    memcpy(identifier, gameUid, byteCount);

    return S_OK;
}
