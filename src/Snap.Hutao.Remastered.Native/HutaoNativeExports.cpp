#pragma once

#include "dllmain.h"
#include "HutaoNativeExports.h"
#include "IHutaoNative.h"
#include <Windows.h>
#include "Types.h"

DLL_EXPORT HRESULT __stdcall HutaoCreateInstance(
	IHutaoNative** ppInstance) {

	if (ppInstance == nullptr) {
		return E_POINTER;
	}


}

DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR error)
{
	return 0;
}
