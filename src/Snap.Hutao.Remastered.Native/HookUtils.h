#pragma once

#include "dllmain.h"
#include <Windows.h>

HRESULT InjectUsingHook(LPCWSTR dllPath, LPCWSTR functionName, DWORD threadId, int hookType);
BOOL IsHookAvailable(int hookType);
LPCWSTR GetHookTypeName(int hookType);