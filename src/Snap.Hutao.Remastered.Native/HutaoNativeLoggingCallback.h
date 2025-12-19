#pragma once

#include "Types.h"
#include "FailureInfo.h"
#include <Windows.h>

struct HutaoNativeLoggingCallback {
	void (__stdcall *value)(FailureInfo*);
};
