#pragma once

#include <Windows.h>
#include "../Snap.Hutao.Remastered.Native/IHutaoNative.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeLoopbackSupport.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeRegistryNotification.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeWindowSubclass.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeWindowNonRude.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeDeviceCapabilities.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativePhysicalDrive.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeLogicalDrive.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeInputLowLevelKeyboardSource.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeFileSystem.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeFileSystem2.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeFileSystem3.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeFileSystem4.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeNotifyIcon.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeHotKeyAction.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeProcess.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoString.h"
#include "../Snap.Hutao.Remastered.Native/HutaoNativeLoggingCallback.h"
#include "../Snap.Hutao.Remastered.Native/HutaoNativeMessageCallback.h"
#include "../Snap.Hutao.Remastered.Native/HutaoNativeHotKeyBeforeSwitchCallback.h"

// Empty callback function definitions
static void __stdcall EmptyMessageCallback(FailureInfo*, PWSTR, ULONG64) {}
static void __stdcall EmptyLoggingCallback(FailureInfo*) {}

extern "C" {

__declspec(dllimport) HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance);
    
__declspec(dllimport) HRESULT __stdcall HutaoStringCreateInstance(
    IHutaoString** ppInstance);
    
__declspec(dllimport) HRESULT __stdcall HutaoNativeRegistryNotificationCreateInstance(
    IHutaoNativeRegistryNotification** ppInstance);
    
__declspec(dllimport) HRESULT __stdcall HutaoInitializeWilCallbacks(
    HutaoNativeLoggingCallback loggingCallback, 
    HutaoNativeMessageCallback messageCallback);
    
__declspec(dllimport) void __stdcall HutaoTestWilCallbacks();

__declspec(dllimport) BOOL __stdcall HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR error);

__declspec(dllimport) HRESULT __stdcall HutaoNativeHotKeyInitializeBeforeSwitchCallback(
    HutaoNativeHotKeyBeforeSwitchCallback callback);

}

// Test helper function declarations
inline HRESULT MakeHutaoNative(IHutaoNative** ppInstance)
{
    return HutaoCreateInstance(ppInstance);
}

inline HRESULT MakeHutaoNative2(IHutaoNative2** ppInstance)
{
    // IHutaoNative2 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative2), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeHutaoNative3(IHutaoNative3** ppInstance)
{
    // IHutaoNative3 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative3), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeHutaoNative4(IHutaoNative4** ppInstance)
{
    // IHutaoNative4 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative4), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeHutaoNative5(IHutaoNative5** ppInstance)
{
    // IHutaoNative5 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative5), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeHutaoNative6(IHutaoNative6** ppInstance)
{
    // IHutaoNative6 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative6), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeHutaoNative7(IHutaoNative7** ppInstance)
{
    // IHutaoNative7 is derived from IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->QueryInterface(__uuidof(IHutaoNative7), (void**)ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeRegistryNotification(IHutaoNativeRegistryNotification** ppInstance)
{
    return HutaoNativeRegistryNotificationCreateInstance(ppInstance);
}

inline HRESULT MakeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData, IHutaoNativeWindowSubclass** ppInstance)
{
    // Create through IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->MakeWindowSubclass(hWnd, callback, userData, ppInstance);
        pNative->Release();
    }
    return hr;
}

inline HRESULT MakeWindowNonRude(HWND hWnd, IHutaoNativeWindowNonRude** ppInstance)
{
    // Create through IHutaoNative
    IHutaoNative* pNative = nullptr;
    HRESULT hr = HutaoCreateInstance(&pNative);
    if (SUCCEEDED(hr))
    {
        hr = pNative->MakeWindowNonRude(hWnd, ppInstance);
        pNative->Release();
    }
    return hr;
}
inline void SetHutaoNativeLoggingCallback(HutaoNativeLoggingCallback callback)
{
    // Use empty message callback
    HutaoNativeMessageCallback emptyMsgCallback = HutaoNativeMessageCallback::Create(EmptyMessageCallback);
    HutaoInitializeWilCallbacks(callback, emptyMsgCallback);
}

inline void SetHutaoNativeMessageCallback(HutaoNativeMessageCallback callback)
{
    // Use empty logging callback
    HutaoNativeLoggingCallback emptyLogCallback = HutaoNativeLoggingCallback::Create(EmptyLoggingCallback);
    HutaoInitializeWilCallbacks(emptyLogCallback, callback);
}
