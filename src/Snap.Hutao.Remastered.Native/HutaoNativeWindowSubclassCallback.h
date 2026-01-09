#pragma once

#include "Types.h"
#include <Windows.h>

using HutaoNativeWindowSubclassProc = BOOL (__stdcall *)(HWND, UINT, WPARAM, LPARAM, GCHandle, LRESULT*);

struct HutaoNativeWindowSubclassCallback
{
private:
    HutaoNativeWindowSubclassProc m_value;
    
public:
    HutaoNativeWindowSubclassCallback() = default;
    
    explicit HutaoNativeWindowSubclassCallback(HutaoNativeWindowSubclassProc method)
        : m_value(method)
    {
    }
    
    static HutaoNativeWindowSubclassCallback Create(HutaoNativeWindowSubclassProc method)
    {
        return HutaoNativeWindowSubclassCallback(method);
    }
    
    HutaoNativeWindowSubclassProc value() const
    {
        return m_value;
    }
    
    bool has_value() const
    {
        return m_value != nullptr;
    }
};
