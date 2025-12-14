#pragma once

#include "ProcessUtils.h"
#include <TlHelp32.h>
#include <Windows.h>
#include <wchar.h>
#include <string.h>

// 获取进程的主线程ID（创建时间最早的线程）
DWORD GetMainThreadId(DWORD processId)
{
    DWORD mainThreadId = 0;
    ULONGLONG minCreateTime = ~(ULONGLONG)0; // 最大可能值

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processId) {
                // 打开线程句柄以获取创建时间
                HANDLE hThread = OpenThread(
                    THREAD_QUERY_INFORMATION,
                    FALSE,
                    te32.th32ThreadID);

                if (hThread) {
                    FILETIME createTime, exitTime, kernelTime, userTime;
                    if (GetThreadTimes(hThread, &createTime, &exitTime,
                        &kernelTime, &userTime)) {
                        ULONGLONG createTime64 = ((ULONGLONG)createTime.dwHighDateTime << 32) |
                            createTime.dwLowDateTime;

                        // 找到创建时间最早的线程（通常是主线程）
                        if (createTime64 < minCreateTime) {
                            minCreateTime = createTime64;
                            mainThreadId = te32.th32ThreadID;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }

    CloseHandle(hSnapshot);
    return mainThreadId;
}

// 查找进程的任意线程ID
DWORD FindAnyThreadId(DWORD processId)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == processId) {
                DWORD threadId = te32.th32ThreadID;
                CloseHandle(hSnapshot);
                return threadId;
            }
        } while (Thread32Next(hSnapshot, &te32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// 获取远程进程中的模块句柄
HMODULE GetRemoteModuleHandle(DWORD processId, LPCWSTR moduleName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    MODULEENTRY32W me32;
    me32.dwSize = sizeof(MODULEENTRY32W);

    HMODULE hModule = NULL;

    if (Module32FirstW(hSnapshot, &me32)) {
        do {
            // 提取模块文件名（不含路径）
            WCHAR moduleFile[MAX_PATH];
            const WCHAR* lastBackslash = wcsrchr(moduleName, L'\\');
            const WCHAR* lastSlash = wcsrchr(moduleName, L'/');
            const WCHAR* fileName = NULL;

            if (lastBackslash || lastSlash) {
                fileName = (lastBackslash > lastSlash) ? lastBackslash + 1 : lastSlash + 1;
            }
            else {
                fileName = (WCHAR*)moduleName;
            }

            // 比较模块名（不区分大小写）
            if (_wcsicmp(me32.szModule, fileName) == 0) {
                hModule = me32.hModule;
                break;
            }
        } while (Module32NextW(hSnapshot, &me32));
    }

    CloseHandle(hSnapshot);
    return hModule;
}

// 检查进程是否在运行
BOOL IsProcessRunning(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (hProcess) {
        DWORD exitCode = 0;
        if (GetExitCodeProcess(hProcess, &exitCode)) {
            CloseHandle(hProcess);
            return (exitCode == STILL_ACTIVE);
        }
        CloseHandle(hProcess);
    }
    return FALSE;
}

// 根据进程名获取进程ID
DWORD GetProcessIdByName(LPCWSTR processName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    DWORD processId = 0;

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                processId = pe32.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processId;
}