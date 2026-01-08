#pragma once

#include "Types.h"
#include "FailureInfo.h"
#include <Windows.h>

using HutaoNativeLoggingProc = void (__stdcall *)(FailureInfo*);

struct HutaoNativeLoggingCallback
{
private:
    HutaoNativeLoggingProc m_value;
    
public:
    HutaoNativeLoggingCallback() = default;
    
    explicit HutaoNativeLoggingCallback(HutaoNativeLoggingProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeLoggingCallback Create(HutaoNativeLoggingProc method)
    {
        return HutaoNativeLoggingCallback(method);
    }
    
    HutaoNativeLoggingProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
