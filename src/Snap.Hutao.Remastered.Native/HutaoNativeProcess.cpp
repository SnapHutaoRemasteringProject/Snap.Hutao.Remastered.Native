#include "HutaoNativeProcess.h"
#include <Windows.h>
#include <string>
#include <vector>

using namespace winrt;

HutaoNativeProcess::HutaoNativeProcess()
    : m_started(false)
    , m_exited(false)
    , m_exitCode(STILL_ACTIVE)
{
    ZeroMemory(&m_processInfo, sizeof(m_processInfo));
    ZeroMemory(&m_startInfo, sizeof(m_startInfo));
}

HutaoNativeProcess::HutaoNativeProcess(HutaoNativeProcessStartInfo info)
    : m_started(false)
    , m_exited(false)
	, m_exitCode(STILL_ACTIVE)
{
    ZeroMemory(&m_processInfo, sizeof(m_processInfo));
    m_startInfo = info;
}

HutaoNativeProcess::~HutaoNativeProcess()
{
    Cleanup();
}

void HutaoNativeProcess::SetStartInfo(const HutaoNativeProcessStartInfo& startInfo)
{
    m_startInfo = startInfo;
}

void HutaoNativeProcess::Cleanup()
{
    if (m_processInfo.hProcess != nullptr)
    {
        CloseHandle(m_processInfo.hProcess);
        m_processInfo.hProcess = nullptr;
    }
    
    if (m_processInfo.hThread != nullptr)
    {
        CloseHandle(m_processInfo.hThread);
        m_processInfo.hThread = nullptr;
    }
    
    m_started = false;
}

BOOL HutaoNativeProcess::CreateProcessInternal()
{
    // 准备启动信息
    STARTUPINFOW startupInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);
    
    // 准备命令行
    std::wstring commandLine;
    if (m_startInfo.CommandLine != nullptr)
    {
        commandLine = m_startInfo.CommandLine;
    }
    else if (m_startInfo.ApplicationName != nullptr)
    {
        commandLine = m_startInfo.ApplicationName;
    }
    
    // 准备当前目录
    LPCWSTR currentDirectory = m_startInfo.CurrentDirectory;
    
    // 创建进程
    BOOL success = CreateProcessW(
        m_startInfo.ApplicationName,           // 应用程序名称
        commandLine.empty() ? nullptr : &commandLine[0], // 命令行
        nullptr,                               // 进程安全属性
        nullptr,                               // 线程安全属性
        m_startInfo.InheritHandles,            // 继承句柄
        m_startInfo.CreationFlags,             // 创建标志
        nullptr,                               // 环境变量
        currentDirectory,                      // 当前目录
        &startupInfo,                          // 启动信息
        &m_processInfo                         // 进程信息
    );
    
    return success;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::Start()
{
    if (m_started)
    {
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }
    
    if (m_startInfo.ApplicationName == nullptr && m_startInfo.CommandLine == nullptr)
    {
        return E_INVALIDARG;
    }
    
    if (!CreateProcessInternal())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    m_started = true;
    m_exited = false;
    m_exitCode = STILL_ACTIVE;
    
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::ResumeMainThread()
{
    if (!m_started || m_processInfo.hThread == nullptr)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    if (ResumeThread(m_processInfo.hThread) == (DWORD)-1)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::WaitForExit()
{
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    DWORD waitResult = WaitForSingleObject(m_processInfo.hProcess, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    // 获取退出码
    if (!::GetExitCodeProcess(m_processInfo.hProcess, &m_exitCode))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    m_exited = true;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::Kill()
{
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    if (!TerminateProcess(m_processInfo.hProcess, 1))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    // 等待进程结束
    WaitForSingleObject(m_processInfo.hProcess, 5000);
    
    // 获取退出码
    if (!::GetExitCodeProcess(m_processInfo.hProcess, &m_exitCode))
    {
        m_exitCode = 1;
    }
    
    m_exited = true;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::GetId(uint* id)
{
    if (id == nullptr)
    {
        return E_POINTER;
    }
    
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        *id = 0;
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    *id = m_processInfo.dwProcessId;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::GetProcessHandle(long long* hProcess)
{
    if (hProcess == nullptr)
    {
        return E_POINTER;
    }
    
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        *hProcess = 0L;
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    // 复制句柄
    HANDLE hProcessCopy;
    if (!DuplicateHandle(
        GetCurrentProcess(),
        m_processInfo.hProcess,
        GetCurrentProcess(),
        &hProcessCopy,
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    *hProcess = (long long)hProcessCopy;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::GetMainWindowHandle(long long* hWnd)
{
    if (hWnd == nullptr)
    {
        return E_POINTER;
    }
    
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        *hWnd = 0L;
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    // 查找进程的主窗口
    // 这是一个简化的实现，实际实现可能需要更复杂的逻辑
    DWORD processId = m_processInfo.dwProcessId;
    
    // 枚举所有窗口，查找属于该进程的窗口
    struct EnumWindowsData
    {
        DWORD processId;
        HWND hWnd;
    } data = { processId, nullptr };
    
    auto enumProc = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        EnumWindowsData* pData = reinterpret_cast<EnumWindowsData*>(lParam);
        
        DWORD windowProcessId;
        GetWindowThreadProcessId(hWnd, &windowProcessId);
        
        if (windowProcessId == pData->processId)
        {
            // 检查窗口是否可见且不是子窗口
            if (IsWindowVisible(hWnd) && GetParent(hWnd) == nullptr)
            {
                pData->hWnd = hWnd;
                return FALSE; // 停止枚举
            }
        }
        
        return TRUE; // 继续枚举
    };
    
    EnumWindows(enumProc, reinterpret_cast<LPARAM>(&data));
    
    *hWnd = (long long)data.hWnd;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE HutaoNativeProcess::GetExitCodeProcess(BOOL* isRunning, uint* exitCode)
{
    if (isRunning == nullptr || exitCode == nullptr)
    {
        return E_POINTER;
    }
    
    if (!m_started || m_processInfo.hProcess == nullptr)
    {
        *isRunning = FALSE;
        *exitCode = 0;
        return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
    }
    
    if (m_exited)
    {
        *isRunning = FALSE;
        *exitCode = m_exitCode;
        return S_OK;
    }
    
    // 检查进程是否还在运行
    DWORD currentExitCode;
    if (!::GetExitCodeProcess(m_processInfo.hProcess, &currentExitCode))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    if (currentExitCode == STILL_ACTIVE)
    {
        *isRunning = TRUE;
        *exitCode = 0;
    }
    else
    {
        m_exited = true;
        m_exitCode = currentExitCode;
        *isRunning = FALSE;
        *exitCode = m_exitCode;
    }
    
    return S_OK;
}
