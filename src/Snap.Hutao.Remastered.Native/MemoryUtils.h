#pragma once

#include <Windows.h>

typedef struct _INJECTION_DATA {
    WCHAR dllPath[MAX_PATH];
    WCHAR functionName[256];
} INJECTION_DATA, * PINJECTION_DATA;

BOOL WriteProcessStringW(HANDLE hProcess, LPCWSTR str, LPVOID* ppRemoteMem);
BOOL WriteProcessData(HANDLE hProcess, LPVOID pRemoteAddress, LPCVOID pData, SIZE_T dataSize);
BOOL ReadProcessData(HANDLE hProcess, LPCVOID pRemoteAddress, LPVOID pBuffer, SIZE_T bufferSize);
PINJECTION_DATA CreateInjectionData(LPCWSTR dllPath, LPCWSTR functionName);
VOID FreeInjectionData(PINJECTION_DATA pData);