#pragma once

#include "Types.h"
#include <Windows.h>

struct HutaoNativeHotKeyBeforeSwitchCallback
{
	BOOL (__stdcall *value)();
};
