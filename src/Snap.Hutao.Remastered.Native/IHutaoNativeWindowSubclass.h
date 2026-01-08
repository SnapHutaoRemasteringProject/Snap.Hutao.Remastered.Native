#pragma once

#include <unknwn.h>

class __declspec(uuid("9631921E-A6CA-4150-9939-99B5467B2FD6"))
IHutaoNativeWindowSubclass : public IUnknown
{
public:
    virtual HRESULT __stdcall Attach() = 0;
    virtual HRESULT __stdcall Detach() = 0;
};

class __declspec(uuid("D3D8B1C9-C83D-472B-AE9C-B65618B0F3AE"))
IHutaoNativeWindowSubclass2 : public IUnknown
{
public:
    virtual HRESULT __stdcall InitializeTaskbarProgress() = 0;
    virtual HRESULT __stdcall SetTaskbarProgress(UINT32 flags, ULONGLONG value, ULONGLONG maximum) = 0;
};
