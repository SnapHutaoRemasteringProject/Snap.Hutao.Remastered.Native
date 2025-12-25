#pragma once

#include "dllmain.h"
#include "Types.h"
#include "HutaoNativeLoggingCallBack.h"
#include "HutaoNativeMessageCallback.h"
#include "IHutaoNative_h.h"
#include "IHutaoString_h.h"
#include "MemoryUtils.h"
#include <Windows.h>

DLL_EXPORT HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance);
DLL_EXPORT HRESULT __stdcall HutaoStringCreateInstance(
    IHutaoString** ppInstance);
DLL_EXPORT HRESULT __stdcall HutaoNativeRegistryNotificationCreateInstance(
    IHutaoNativeRegistryNotification** ppInstance);
DLL_EXPORT HRESULT HutaoInitializeWilCallbacks(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback);
DLL_EXPORT void HutaoTestWilCallbacks();
DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR  error);

