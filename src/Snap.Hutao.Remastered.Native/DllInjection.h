#pragma once

#include "dllmain.h"
#include "types.h"
#include "ProcessUtils.h"
#include "MemoryUtils.h"
#include "PrivilegeUtils.h"
#include "HookUtils.h"

    // 主导出函数
DLL_EXPORT HRESULT DllInjectionUtilitiesInjectUsingWindowsHook(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT DllInjectionUtilitiesInjectUsingWindowsHook2(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT DllInjectionUtilitiesInjectUsingRemoteThread(
        LPCWSTR dllPath,
        int processId);

DLL_EXPORT HRESULT DllInjectionUtilitiesInjectUsingRemoteThreadWithFunction(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);
