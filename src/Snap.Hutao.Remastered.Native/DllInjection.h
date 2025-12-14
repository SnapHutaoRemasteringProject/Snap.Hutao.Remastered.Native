#pragma once

#include "dllmain.h"
#include <Windows.h>

    // 主导出函数
DLL_EXPORT HRESULT WINAPI DllInjectionUtilitiesInjectUsingWindowsHook(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT WINAPI DllInjectionUtilitiesInjectUsingWindowsHook2(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);

DLL_EXPORT HRESULT WINAPI DllInjectionUtilitiesInjectUsingRemoteThread(
        LPCWSTR dllPath,
        int processId);

DLL_EXPORT HRESULT WINAPI DllInjectionUtilitiesInjectUsingRemoteThreadWithFunction(
        LPCWSTR dllPath,
        LPCWSTR functionName,
        int processId);
