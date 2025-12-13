#pragma once

#include "dllmain.h"

BOOL EnableDebugPrivilege();
BOOL EnablePrivilege(LPCWSTR privilegeName);
BOOL IsRunningAsAdministrator();
