#pragma once

#include "Types.h"
#include <Windows.h>

using HutaoNativeHotKeyActionProc = void (__stdcall *)(BOOL isOn, GCHandle userData);

struct HutaoNativeHotKeyActionCallback
{
private:
    HutaoNativeHotKeyActionProc m_value;
    
public:
    HutaoNativeHotKeyActionCallback() = default;
    
    explicit HutaoNativeHotKeyActionCallback(HutaoNativeHotKeyActionProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeHotKeyActionCallback Create(HutaoNativeHotKeyActionProc method)
    {
        return HutaoNativeHotKeyActionCallback(method);
    }
    
    HutaoNativeHotKeyActionProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
