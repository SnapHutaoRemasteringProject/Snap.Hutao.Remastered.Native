#pragma once

#include <Windows.h>
#include "dllmain.h"
#include "Types.h"

typedef struct _INJECTION_DATA {
    WCHAR dllPath[MAX_PATH];
    WCHAR functionName[256];
} INJECTION_DATA, * PINJECTION_DATA;

BOOL WriteProcessStringW(HANDLE hProcess, LPCWSTR str, LPVOID* ppRemoteMem);
BOOL WriteProcessData(HANDLE hProcess, LPVOID pRemoteAddress, LPCVOID pData, SIZE_T dataSize);
BOOL ReadProcessData(HANDLE hProcess, LPCVOID pRemoteAddress, LPVOID pBuffer, SIZE_T bufferSize);
PINJECTION_DATA CreateInjectionData(LPCWSTR dllPath, LPCWSTR functionName);
VOID FreeInjectionData(PINJECTION_DATA pData);

// Patch callback function pointer type
typedef HRESULT (__stdcall* PatchCallbackFunc)(byte* ptr, int size, GCHandle state);

// Memory utilities patch function
DLL_EXPORT HRESULT __stdcall MemoryUtilitiesPatch(
    PCWSTR moduleName, 
    uint offset, 
    int size, 
    PatchCallbackFunc callback, 
    GCHandle state);
