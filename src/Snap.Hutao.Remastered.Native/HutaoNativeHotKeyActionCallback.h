#pragma once

#include "Types.h"
#include <Windows.h>

typedef void (__stdcall *HutaoNativeHotKeyActionCallbackFunc)(BOOL, GCHandle);

struct HutaoNativeHotKeyActionCallback
{
    HutaoNativeHotKeyActionCallbackFunc value;
};
