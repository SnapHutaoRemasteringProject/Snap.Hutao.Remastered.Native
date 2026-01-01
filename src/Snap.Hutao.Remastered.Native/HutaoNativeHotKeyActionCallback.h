#pragma once

#include "Types.h"
#include <Windows.h>

struct HutaoNativeHotKeyActionCallback
{
	void (__stdcall *value)(BOOL isOn, LONG_PTR userData);
};
