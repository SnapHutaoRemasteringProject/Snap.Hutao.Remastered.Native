#pragma once

#include "FailureInfo.h"
#include <Windows.h>

struct HutaoNativeMessageCallback {
	void (__stdcall *value) (FailureInfo*, PWSTR, ULONG64);
};