#pragma once

#include <unknwn.h>
#include "types.h"

class __declspec(uuid("13A2B1A2-03CB-46E1-AED5-6AFEA2DFFB39"))
IHutaoNativeProcess : public IUnknown
{
public:
    virtual HRESULT __stdcall Start() = 0;
    virtual HRESULT __stdcall ResumeMainThread() = 0;
    virtual HRESULT __stdcall WaitForExit() = 0;
    virtual HRESULT __stdcall Kill() = 0;
    virtual HRESULT __stdcall GetId(uint* id) = 0;
    virtual HRESULT __stdcall GetProcessHandle(long long* hProcess) = 0;
    virtual HRESULT __stdcall GetMainWindowHandle(long long* hWnd) = 0;
    virtual HRESULT __stdcall GetExitCodeProcess(BOOL* isRunning, uint* exitCode) = 0;
};
