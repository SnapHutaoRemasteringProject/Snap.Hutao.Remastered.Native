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

using HutaoNativeNotifyIconProc = void (*)(HutaoNativeNotifyIconCallbackKind kind, RECT rect, POINT point, GCHandle userData);
struct HutaoNativeNotifyIconCallback
{
private:
    HutaoNativeNotifyIconProc m_value;
    
public:
    HutaoNativeNotifyIconCallback() = default;
    
    explicit HutaoNativeNotifyIconCallback(HutaoNativeNotifyIconProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeNotifyIconCallback Create(HutaoNativeNotifyIconProc method)
    {
        return HutaoNativeNotifyIconCallback(method);
    }
    
    HutaoNativeNotifyIconProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
