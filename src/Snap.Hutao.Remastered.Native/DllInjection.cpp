#pragma once

#include "DllInjection.h"
#include "ProcessUtils.h"
#include "PrivilegeUtils.h"
#include "HookUtils.h"
#include "MemoryUtils.h"
#include "StringUtils.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <tlhelp32.h>

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

    // 打开目标进程
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);

    if (!hProcess) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = S_OK;
    LPVOID pRemoteDllPath = NULL;
    HMODULE hRemoteModule = NULL;
    HMODULE hLocalModule = NULL;
    FARPROC pLocalFunction = NULL;
    LPVOID pRemoteFunction = NULL;
    HANDLE hRemoteThread = NULL;
    LPCSTR ansiFunctionName = NULL;
    DWORD exitCode = 0;
    std::wstring wDllPath;
    size_t lastSlash = 0;
    std::wstring wDllName;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    MODULEENTRY32W me32 = { 0 };
    bool found = false;
    ULONG_PTR localBase = 0;
    ULONG_PTR remoteBase = 0;
    ULONG_PTR localFunctionAddr = 0;

    // 在目标进程中分配内存并写入DLL路径
    if (!WriteProcessStringW(hProcess, dllPath, &pRemoteDllPath)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(hProcess);
        return hr;
    }

    // 获取LoadLibraryW函数地址
    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

    if (!pLoadLibraryW) {
        hr = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
        goto cleanup;
    }

    // 创建远程线程调用LoadLibraryW加载DLL
    hRemoteThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryW, pRemoteDllPath, 0, NULL);

    if (!hRemoteThread) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }

    // 等待线程完成
    WaitForSingleObject(hRemoteThread, 5000); // 5秒超时
    
    GetExitCodeThread(hRemoteThread, &exitCode);
    CloseHandle(hRemoteThread);
    hRemoteThread = NULL;

    if (exitCode == 0) {
        hr = E_FAIL;
        goto cleanup;
    }

    // 等待DLL加载完成
    Sleep(200);

    // 使用Toolhelp32枚举远程进程模块找到DLL的基址
    // 首先需要将函数名从宽字符转换为多字节（UTF-8）
    // 使用STL vector进行字符串处理
    
    // 获取DLL文件名（不含路径）
    wDllPath = std::wstring(dllPath);
    lastSlash = wDllPath.find_last_of(L"\\/");
    wDllName = (lastSlash != std::wstring::npos) ? 
        wDllPath.substr(lastSlash + 1) : wDllPath;
    
    // 枚举远程进程模块
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }
    
    me32.dwSize = sizeof(MODULEENTRY32W);
    
    if (!Module32FirstW(hSnapshot, &me32)) {
        CloseHandle(hSnapshot);
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }
    
    found = false;
    do {
        if (_wcsicmp(me32.szModule, wDllName.c_str()) == 0 ||
            _wcsicmp(me32.szExePath, wDllPath.c_str()) == 0) {
            hRemoteModule = me32.hModule;
            found = true;
            break;
        }
    } while (Module32NextW(hSnapshot, &me32));
    
    CloseHandle(hSnapshot);
    hSnapshot = INVALID_HANDLE_VALUE;
    
    if (!found) {
        // DLL可能已加载但未找到模块信息
        hr = S_OK; // 返回成功，因为DLL已加载，但无法调用指定函数
        goto cleanup;
    }

    // 在本地加载相同的DLL
    hLocalModule = LoadLibraryW(dllPath);
    if (!hLocalModule) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }

    // 在本地DLL中获取目标函数地址
    // 使用StringUtils中的辅助函数获取函数名
    ansiFunctionName = GetAnsiFunctionName(functionName);
    if (!ansiFunctionName) {
        hr = E_OUTOFMEMORY;
        goto cleanup;
    }
    
    pLocalFunction = GetProcAddress(hLocalModule, ansiFunctionName);
    if (!pLocalFunction) {
        hr = HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
        goto cleanup;
    }

    // 计算远程函数地址：远程基址 + (本地函数地址 - 本地基址)
    localBase = (ULONG_PTR)hLocalModule;
    remoteBase = (ULONG_PTR)hRemoteModule;
    localFunctionAddr = (ULONG_PTR)pLocalFunction;
    
    pRemoteFunction = (LPVOID)(remoteBase + (localFunctionAddr - localBase));

    // 创建远程线程调用目标函数
    hRemoteThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)pRemoteFunction, NULL, 0, NULL);

    if (!hRemoteThread) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto cleanup;
    }

    // 等待线程完成
    WaitForSingleObject(hRemoteThread, 5000); // 5秒超时
    
    GetExitCodeThread(hRemoteThread, &exitCode);
    CloseHandle(hRemoteThread);
    hRemoteThread = NULL;

    hr = (exitCode != 0) ? S_OK : E_FAIL;

cleanup:
    // 清理资源
    if (hRemoteThread) {
        CloseHandle(hRemoteThread);
    }
    
    if (hLocalModule) {
        FreeLibrary(hLocalModule);
    }
    
    if (ansiFunctionName) {
        FreeConvertedString((LPVOID)ansiFunctionName);
    }
    
    if (pRemoteDllPath) {
        VirtualFreeEx(hProcess, pRemoteDllPath, 0, MEM_RELEASE);
    }
    
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshot);
    }
    
    CloseHandle(hProcess);
    
    return hr;
}
