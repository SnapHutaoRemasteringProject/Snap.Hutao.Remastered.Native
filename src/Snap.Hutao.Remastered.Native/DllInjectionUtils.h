#pragma once

#include "dllmain.h"
#include <Windows.h>

    // 主导出函数
DLL_EXPORT HRESULT __stdcall DllInjectionUtilitiesInjectUsingWindowsHook(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT __stdcall DllInjectionUtilitiesInjectUsingWindowsHook2(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT __stdcall DllInjectionUtilitiesInjectUsingRemoteThread(
        LPCWSTR dllPath,
        int processId);

DLL_EXPORT HRESULT __stdcall DllInjectionUtilitiesInjectUsingRemoteThreadWithFunction(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);
