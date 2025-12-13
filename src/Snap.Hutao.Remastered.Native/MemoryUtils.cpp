#include "MemoryUtils.h"

// 向目标进程写入字符串并返回远程内存地址
BOOL WriteProcessStringW(HANDLE hProcess, LPCWSTR str, LPVOID* ppRemoteMem)
{
    if (!hProcess || !str || !ppRemoteMem) {
        return FALSE;
    }

    // 计算字符串长度（包含终止符）
    SIZE_T strLen = (wcslen(str) + 1) * sizeof(WCHAR);

    // 在目标进程中分配内存
    LPVOID pRemoteMem = VirtualAllocEx(hProcess, NULL, strLen,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!pRemoteMem) {
        return FALSE;
    }

    // 写入字符串到目标进程
    SIZE_T bytesWritten = 0;
    BOOL success = WriteProcessMemory(hProcess, pRemoteMem, str, strLen, &bytesWritten);

    if (!success || bytesWritten != strLen) {
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }

    *ppRemoteMem = pRemoteMem;
    return TRUE;
}

// 向目标进程写入数据
BOOL WriteProcessData(HANDLE hProcess, LPVOID pRemoteAddress, LPCVOID pData, SIZE_T dataSize)
{
    if (!hProcess || !pRemoteAddress || !pData || dataSize == 0) {
        return FALSE;
    }

    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(hProcess, pRemoteAddress, pData, dataSize, &bytesWritten) &&
        bytesWritten == dataSize;
}

// 从目标进程读取数据
BOOL ReadProcessData(HANDLE hProcess, LPCVOID pRemoteAddress, LPVOID pBuffer, SIZE_T bufferSize)
{
    if (!hProcess || !pRemoteAddress || !pBuffer || bufferSize == 0) {
        return FALSE;
    }

    SIZE_T bytesRead = 0;
    return ReadProcessMemory(hProcess, pRemoteAddress, pBuffer, bufferSize, &bytesRead) &&
        bytesRead == bufferSize;
}

// 创建注入数据结构
PINJECTION_DATA CreateInjectionData(LPCWSTR dllPath, LPCWSTR functionName)
{
    PINJECTION_DATA pData = (PINJECTION_DATA)malloc(sizeof(INJECTION_DATA));
    if (!pData) {
        return NULL;
    }

    wcsncpy_s(pData->dllPath, MAX_PATH, dllPath, _TRUNCATE);
    wcsncpy_s(pData->functionName, 256, functionName, _TRUNCATE);

    return pData;
}

// 释放注入数据结构
VOID FreeInjectionData(PINJECTION_DATA pData)
{
    if (pData) {
        free(pData);
    }
}