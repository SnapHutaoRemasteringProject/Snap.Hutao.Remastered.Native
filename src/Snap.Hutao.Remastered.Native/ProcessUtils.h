#pragma once

#include "dllmain.h"
#include <TlHelp32.h>

DWORD GetMainThreadId(DWORD processId);
DWORD FindAnyThreadId(DWORD processId);
HMODULE GetRemoteModuleHandle(DWORD processId, LPCWSTR moduleName);
BOOL IsProcessRunning(DWORD processId);
DWORD GetProcessIdByName(LPCWSTR processName);