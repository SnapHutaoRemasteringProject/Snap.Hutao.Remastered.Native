#pragma once

#include "Types.h"
#include <Windows.h>

// 通知图标回调种类枚举
enum class HutaoNativeNotifyIconCallbackKind : int
{
    TaskbarCreated = 0,
    ContextMenu = 1,
    LeftButtonDown = 2,
    LeftButtonDoubleClick = 3,
};

// 通知图标回调函数指针类型
typedef void (*HutaoNativeNotifyIconCallbackFunc)(HutaoNativeNotifyIconCallbackKind kind, RECT rect, POINT point, nint userData);

// 通知图标回调结构体（模仿项目风格）
struct HutaoNativeNotifyIconCallback
{
    HutaoNativeNotifyIconCallbackFunc value;
};
