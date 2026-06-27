#pragma once
#include <Windows.h>
#include <unknwn.h>
#include "IHutaoString.h"

// {E8F9A1B2-C3D4-5678-9ABC-DEF012345678}
class __declspec(uuid("E8F9A1B2-C3D4-5678-9ABC-DEF012345678")) IHutaoNativeFileSystem5 : public IUnknown
{
public:
    virtual HRESULT __stdcall ResolveLink(PCWSTR lnkPath, IHutaoString** targetPath) = 0;
};
