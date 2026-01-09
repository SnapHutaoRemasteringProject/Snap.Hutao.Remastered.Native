#pragma once

#include "IHutaoNativeProcess.h"
#include "Types.h"
#include "CustomImplements.h"
#include <Windows.h>

class HutaoNativeProcess : public hutao::CustomImplements<
    HutaoNativeProcess,
    IHutaoNativeProcess>
{
public:
    HutaoNativeProcess();
    HutaoNativeProcess(HutaoNativeProcessStartInfo info);
    ~HutaoNativeProcess();

    // IHutaoNativeProcess methods
    virtual HRESULT __stdcall Start() override;
    virtual HRESULT __stdcall ResumeMainThread() override;
    virtual HRESULT __stdcall WaitForExit() override;
    virtual HRESULT __stdcall Kill() override;
    virtual HRESULT __stdcall GetId(uint* id) override;
    virtual HRESULT __stdcall GetProcessHandle(long long* hProcess) override;
    virtual HRESULT __stdcall GetMainWindowHandle(long long* hWnd) override;
    virtual HRESULT __stdcall GetExitCodeProcess(BOOL* isRunning, uint* exitCode) override;

    // 设置启动信息
    void SetStartInfo(const HutaoNativeProcessStartInfo& startInfo);

private:
    HutaoNativeProcessStartInfo m_startInfo;
    PROCESS_INFORMATION m_processInfo;
    bool m_started;
    bool m_exited;
    DWORD m_exitCode;

    void Cleanup();
    BOOL CreateProcessInternal();
};
