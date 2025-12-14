#pragma once

#include "HookUtils.h"
#include "StringUtils.h"
#include <Windows.h>

// 使用Windows钩子注入DLL
HRESULT InjectUsingHook(LPCWSTR dllPath, LPCWSTR functionName, DWORD threadId, int hookType)
{
    if (!dllPath || !functionName) {
        return E_INVALIDARG;
    }

    HMODULE hHookDll = LoadLibraryW(dllPath);
    if (!hHookDll) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    LPCSTR ansiFunctionName = GetAnsiFunctionName(functionName);
    if (!ansiFunctionName) {
        FreeLibrary(hHookDll);
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
    }

    HOOKPROC hookProc = NULL;

    if ((DWORD_PTR)ansiFunctionName <= 0xFFFF) {
        // 是序号
        hookProc = (HOOKPROC)GetProcAddress(hHookDll, ansiFunctionName);
    }
    else {
        // 是字符串指针
        hookProc = (HOOKPROC)GetProcAddress(hHookDll, ansiFunctionName);

        // 释放转换的字符串
        FreeConvertedString((LPVOID)ansiFunctionName);
    }

    if (!hookProc) {
        FreeLibrary(hHookDll);
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }

    // 设置Windows钩子
    HHOOK hHook = SetWindowsHookExW(
        hookType,           // 钩子类型
        hookProc,           // 钩子过程
        hHookDll,           // 包含钩子过程的DLL
        threadId            // 目标线程ID（0表示全局钩子）
    );

    if (!hHook) {
        DWORD lastError = GetLastError();
        FreeLibrary(hHookDll);
        return HRESULT_FROM_WIN32(lastError);
    }

    // 触发钩子 - 发送消息到目标线程的消息队列
    if (threadId != 0) {
        // 线程特定的钩子，发送线程消息
        PostThreadMessageW(threadId, WM_NULL, 0, 0);
    }
    else {
        // 全局钩子，发送广播消息
        PostMessageW(HWND_BROADCAST, WM_NULL, 0, 0);
    }

    // 等待一段时间确保DLL被加载
    Sleep(100);

    // 卸载钩子（DLL已注入，不需要保持钩子）
    UnhookWindowsHookEx(hHook);

    // 释放当前进程中的DLL（不影响已注入到目标进程的DLL）
    FreeLibrary(hHookDll);

    return S_OK;
}

// 检查钩子是否可用
BOOL IsHookAvailable(int hookType)
{
    // 临时钩子过程
    HOOKPROC tempProc = [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
        };

    // 尝试设置一个临时钩子来检查是否可用
    HHOOK hHook = SetWindowsHookExW(hookType, tempProc, NULL, 0);

    if (hHook) {
        UnhookWindowsHookEx(hHook);
        return TRUE;
    }

    return FALSE;
}

// 获取钩子类型名称
LPCWSTR GetHookTypeName(int hookType)
{
    switch (hookType) {
    case WH_GETMESSAGE:
        return L"WH_GETMESSAGE";
    case WH_CALLWNDPROC:
        return L"WH_CALLWNDPROC";
    case WH_KEYBOARD:
        return L"WH_KEYBOARD";
    case WH_MOUSE:
        return L"WH_MOUSE";
    case WH_KEYBOARD_LL:
        return L"WH_KEYBOARD_LL";
    case WH_MOUSE_LL:
        return L"WH_MOUSE_LL";
    case WH_SHELL:
        return L"WH_SHELL";
    case WH_FOREGROUNDIDLE:
        return L"WH_FOREGROUNDIDLE";
    case WH_CBT:
        return L"WH_CBT";
    case WH_MSGFILTER:
        return L"WH_MSGFILTER";
    case WH_JOURNALRECORD:
        return L"WH_JOURNALRECORD";
    case WH_JOURNALPLAYBACK:
        return L"WH_JOURNALPLAYBACK";
    case WH_SYSMSGFILTER:
        return L"WH_SYSMSGFILTER";
    default:
        return L"UNKNOWN";
    }
}