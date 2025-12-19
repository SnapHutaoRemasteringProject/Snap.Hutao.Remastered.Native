#pragma once

#include "Types.h"
#include <Windows.h>

struct HutaoNativeWindowSubclassCallback
{
	BOOL (*value)(HWND, UINT, WPARAM, LPARAM, GCHandle, LRESULT*);
};