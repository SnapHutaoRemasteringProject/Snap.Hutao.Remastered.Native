#pragma once

#include "dllmain.h"
#include "IHutaoNative.h"
#include "Types.h"
#include <Windows.h>

DLL_EXPORT HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance);
DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR  error);