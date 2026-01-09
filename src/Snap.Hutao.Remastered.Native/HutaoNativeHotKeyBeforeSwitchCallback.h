#pragma once

#include "Types.h"
#include <Windows.h>

using HutaoNativeHotKeyBeforeSwitchProc = BOOL (__stdcall *)();

struct HutaoNativeHotKeyBeforeSwitchCallback
{
private:
    HutaoNativeHotKeyBeforeSwitchProc m_value;
    
public:
    HutaoNativeHotKeyBeforeSwitchCallback() = default;
    
    explicit HutaoNativeHotKeyBeforeSwitchCallback(HutaoNativeHotKeyBeforeSwitchProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeHotKeyBeforeSwitchCallback Create(HutaoNativeHotKeyBeforeSwitchProc method)
    {
        return HutaoNativeHotKeyBeforeSwitchCallback(method);
    }
    
    HutaoNativeHotKeyBeforeSwitchProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
