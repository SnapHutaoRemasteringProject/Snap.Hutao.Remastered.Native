#pragma once

#include "FailureInfo.h"
#include <Windows.h>

using HutaoNativeMessageProc = void (__stdcall *)(FailureInfo*, PWSTR, ULONG64);

struct HutaoNativeMessageCallback
{
private:
    HutaoNativeMessageProc m_value;
    
public:
    HutaoNativeMessageCallback() = default;
    
    explicit HutaoNativeMessageCallback(HutaoNativeMessageProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeMessageCallback Create(HutaoNativeMessageProc method)
    {
        return HutaoNativeMessageCallback(method);
    }
    
    HutaoNativeMessageProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
