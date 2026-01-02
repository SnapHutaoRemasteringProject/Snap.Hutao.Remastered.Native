#pragma once

#include "Types.h"
#include <Windows.h>

enum class HutaoNativeNotifyIconCallbackKind : int
{
    TaskbarCreated = 0,
    ContextMenu = 1,
    LeftButtonDown = 2,
    LeftButtonDoubleClick = 3,
};

typedef void (*HutaoNativeNotifyIconCallbackFunc)(HutaoNativeNotifyIconCallbackKind kind, RECT rect, POINT point, nint userData);

struct HutaoNativeNotifyIconCallback
{
    HutaoNativeNotifyIconCallbackFunc value;
};
