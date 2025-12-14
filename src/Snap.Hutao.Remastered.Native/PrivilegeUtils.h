#pragma once

#include <Windows.h>

BOOL EnableDebugPrivilege();
BOOL EnablePrivilege(LPCWSTR privilegeName);
BOOL IsRunningAsAdministrator();
