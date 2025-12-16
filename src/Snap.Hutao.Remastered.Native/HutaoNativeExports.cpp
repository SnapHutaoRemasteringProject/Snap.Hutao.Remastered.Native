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

	// Not implemented: return null instance and a not-implemented HRESULT.
	*ppInstance = nullptr;
	return E_NOTIMPL;
}

DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR error)
{
	// Compare HRESULT with HRESULT_FROM_WIN32(error)
	return hr == HRESULT_FROM_WIN32(error) ? TRUE : FALSE;
}
