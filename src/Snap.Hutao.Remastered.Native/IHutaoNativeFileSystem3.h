#pragma once

#include <Windows.h>
#include <unknwn.h>
#include "IHutaoString.h"

// {6DBCFC5C-2555-44DB-AF9D-2A2628CF726E}
class __declspec(uuid("6DBCFC5C-2555-44DB-AF9D-2A2628CF726E")) IHutaoNativeFileSystem3 : public IUnknown
{
public:
    virtual HRESULT __stdcall PickFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) = 0;
    virtual HRESULT __stdcall SaveFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) = 0;
    virtual HRESULT __stdcall PickFolder(HWND hwnd, PCWSTR title, BOOL* picked, IHutaoString** path) = 0;
};
