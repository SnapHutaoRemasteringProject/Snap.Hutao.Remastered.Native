#pragma once

#include "IHutaoNativeProcess_h.h"
#include "Types_h.h"
#include <winrt/base.h>
#include <Windows.h>

class HutaoNativeProcess : public winrt::implements<
    HutaoNativeProcess,
    IHutaoNativeProcess,
    winrt::non_agile>
{
public:
    HutaoNativeProcess();
    HutaoNativeProcess(HutaoNativeProcessStartInfo info);
    ~HutaoNativeProcess();

    // IHutaoNativeProcess methods
    virtual HRESULT STDMETHODCALLTYPE Start() override;
    virtual HRESULT STDMETHODCALLTYPE ResumeMainThread() override;
    virtual HRESULT STDMETHODCALLTYPE WaitForExit() override;
    virtual HRESULT STDMETHODCALLTYPE Kill() override;
    virtual HRESULT STDMETHODCALLTYPE GetId(uint* id) override;
    virtual HRESULT STDMETHODCALLTYPE GetProcessHandle(long long* hProcess) override;
    virtual HRESULT STDMETHODCALLTYPE GetMainWindowHandle(long long* hWnd) override;
    virtual HRESULT STDMETHODCALLTYPE GetExitCodeProcess(BOOL* isRunning, uint* exitCode) override;

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
