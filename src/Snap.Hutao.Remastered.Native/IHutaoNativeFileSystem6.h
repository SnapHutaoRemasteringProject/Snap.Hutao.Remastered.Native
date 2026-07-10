#pragma once
#include <Windows.h>
#include <unknwn.h>

// {6B2C8D91-4A3F-4E7B-B1C5-9F0E2D3A4B8C}
class __declspec(uuid("6B2C8D91-4A3F-4E7B-B1C5-9F0E2D3A4B8C")) IHutaoNativeFileSystem6 : public IUnknown
{
public:
    virtual HRESULT __stdcall CreateLinkWithAppUserModelId(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName, PCWSTR appUserModelId) = 0;
};
