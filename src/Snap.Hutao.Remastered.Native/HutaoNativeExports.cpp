#pragma once

#include "dllmain.h"
#include "HutaoNativeExports.h"
#include "WilCallbacksManager.h"
#include "Types.h"
#include "HutaoNative.h"
#include "HutaoString.h"
#include "HutaoNativeRegistryNotification.h"
#include "HutaoNativeLoggingCallback.h"
#include "HutaoNativeMessageCallback.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNative_h.h"
#include "IHutaoString_h.h"

#include <Windows.h>
#include <winrt/base.h>

using namespace winrt;

DLL_EXPORT HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance) {

    if (ppInstance == nullptr) {
        return E_POINTER;
    }

    try
    {
        winrt::init_apartment();
        com_ptr<IHutaoNative> instance = make_self<HutaoNative>();
        *ppInstance = detach_abi(instance);
        return S_OK;
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
}

DLL_EXPORT HRESULT __stdcall HutaoStringCreateInstance(
    IHutaoString** ppInstance) {

    if (ppInstance == nullptr) {
        return E_POINTER;
    }

    try
    {
        winrt::init_apartment();
        com_ptr<IHutaoString> instance = make_self<HutaoString>();
        *ppInstance = detach_abi(instance);
        return S_OK;
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
}

DLL_EXPORT HRESULT __stdcall HutaoNativeRegistryNotificationCreateInstance(IHutaoNativeRegistryNotification** ppInstance)
{
    if (ppInstance == nullptr) {
        return E_POINTER;
    }

    try
    {
        winrt::init_apartment();
        com_ptr<IHutaoNativeRegistryNotification> instance = make_self<HutaoNativeRegistryNotification>();
        *ppInstance = detach_abi(instance);
        return S_OK;
    }
    catch (...)
    {
        return winrt::to_hresult();
    }
}

DLL_EXPORT HRESULT HutaoInitializeWilCallbacks(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback)
{
	g_wilCallbacksManager.Initialize(loggingCallback, messageCallback);
    return S_OK;
}

DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR error)
{
    // Compare HRESULT with HRESULT_FROM_WIN32(error)
    return hr == HRESULT_FROM_WIN32(error) ? TRUE : FALSE;
}
