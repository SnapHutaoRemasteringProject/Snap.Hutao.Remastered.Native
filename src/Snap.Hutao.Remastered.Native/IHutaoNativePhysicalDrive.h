#pragma once
#include <Windows.h>
#include <unknwn.h>
#include "types.h"

// {F90535D7-AFF6-4008-BA8C-15C47FC9660D}
class __declspec(uuid("F90535D7-AFF6-4008-BA8C-15C47FC9660D")) IHutaoNativePhysicalDrive : public IUnknown
{
public:
    virtual HRESULT __stdcall IsPathOnSolidStateDrive(PCWSTR root, BOOL* isSSD) = 0;
};
