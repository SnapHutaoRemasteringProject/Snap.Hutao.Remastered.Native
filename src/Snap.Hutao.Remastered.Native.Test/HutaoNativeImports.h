#pragma once

#include <Windows.h>
#include "../Snap.Hutao.Remastered.Native/IHutaoNative_h.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoNativeRegistryNotification_h.h"
#include "../Snap.Hutao.Remastered.Native/IHutaoString_h.h"

#define DLL_IMPORT __declspec(dllimport) extern "C"

DLL_IMPORT HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance);
DLL_IMPORT HRESULT __stdcall HutaoStringCreateInstance(
    IHutaoString** ppInstance);
DLL_IMPORT HRESULT __stdcall HutaoNativeRegistryNotificationCreateInstance(
    IHutaoNativeRegistryNotification** ppInstance);