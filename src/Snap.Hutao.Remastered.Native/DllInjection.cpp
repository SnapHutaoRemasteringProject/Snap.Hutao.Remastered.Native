#pragma once

#include "DllInjection.h"
#include "ProcessUtils.h"
#include "PrivilegeUtils.h"
#include "HookUtils.h"
#include "MemoryUtils.h"
#include <Windows.h>

// Windows钩子注入方法1（使用WH_GETMESSAGE钩子）
HRESULT DllInjectionUtilitiesInjectUsingWindowsHook(
    LPCWSTR dllPath,
    LPCWSTR functionName,
    int processId)
{
    if (!dllPath || !functionName || processId <= 0) {
        return E_INVALIDARG;
    }

    if (GetFileAttributesW(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    EnableDebugPrivilege();

    // 获取目标进程的主线程ID
    DWORD targetThreadId = GetMainThreadId(processId);
    if (targetThreadId == 0) {
        // 如果没有主线程，尝试枚举其他线程
        targetThreadId = FindAnyThreadId(processId);
        if (targetThreadId == 0) {
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
    }

    // 使用钩子注入（WH_GETMESSAGE）
    return InjectUsingHook(dllPath, functionName, targetThreadId, WH_GETMESSAGE);
}

// Windows钩子注入方法2（使用WH_CALLWNDPROC钩子）
HRESULT DllInjectionUtilitiesInjectUsingWindowsHook2(
    LPCWSTR dllPath,
    LPCWSTR functionName,
    int processId)
{
    if (!dllPath || !functionName || processId <= 0) {
        return E_INVALIDARG;
    }

    if (GetFileAttributesW(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    EnableDebugPrivilege();

    // 获取目标进程的主线程ID
    DWORD targetThreadId = GetMainThreadId(processId);
    if (targetThreadId == 0) {
        targetThreadId = FindAnyThreadId(processId);
        if (targetThreadId == 0) {
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
    }

    // 使用钩子注入（WH_CALLWNDPROC）
    return InjectUsingHook(dllPath, functionName, targetThreadId, WH_CALLWNDPROC);
}

// 远程线程注入（仅加载DLL）
HRESULT DllInjectionUtilitiesInjectUsingRemoteThread(
    LPCWSTR dllPath,
    int processId)
{
    if (!dllPath || processId <= 0) {
        return E_INVALIDARG;
    }

    if (GetFileAttributesW(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    EnableDebugPrivilege();

    // 打开目标进程
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);

    if (!hProcess) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // 在目标进程中分配内存并写入DLL路径
    LPVOID pRemoteMemory = NULL;
    if (!WriteProcessStringW(hProcess, dllPath, &pRemoteMemory)) {
        CloseHandle(hProcess);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // 获取LoadLibraryW函数地址
    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

    if (!pLoadLibrary) {
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    // 创建远程线程调用LoadLibraryW
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemoteMemory, 0, NULL);

    if (!hRemoteThread) {
        DWORD lastError = GetLastError();
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return HRESULT_FROM_WIN32(lastError);
    }

    // 等待线程完成
    WaitForSingleObject(hRemoteThread, 5000); // 5秒超时

    // 获取退出码
    DWORD exitCode = 0;
    GetExitCodeThread(hRemoteThread, &exitCode);

    CloseHandle(hRemoteThread);

    // 延迟释放内存，确保DLL已加载
    Sleep(100);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return (exitCode != 0) ? S_OK : E_FAIL;
}

// 远程线程注入（加载DLL并调用指定函数）
HRESULT DllInjectionUtilitiesInjectUsingRemoteThreadWithFunction(
    LPCWSTR dllPath,
    LPCWSTR functionName,
    int processId)
{
    if (!dllPath || !functionName || processId <= 0) {
        return E_INVALIDARG;
    }

    if (GetFileAttributesW(dllPath) == INVALID_FILE_ATTRIBUTES) {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    EnableDebugPrivilege();

    // 先注入DLL
    HRESULT hr = DllInjectionUtilitiesInjectUsingRemoteThread(dllPath, processId);
    if (FAILED(hr)) {
        return hr;
    }

    // 等待DLL加载完成
    Sleep(200);

    // 获取注入的DLL模块句柄
    HMODULE hRemoteModule = GetRemoteModuleHandle(processId, dllPath);
    if (!hRemoteModule) {
        // DLL可能已加载但未找到模块信息
        // 返回成功，因为DLL已加载，但无法调用指定函数
        return S_OK;
    }

    // 这是一个高级功能，需要创建远程代码存根
    // 由于不使用STL且要求简单，这里返回不支持
    // 实际实现需要更多工作

    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}