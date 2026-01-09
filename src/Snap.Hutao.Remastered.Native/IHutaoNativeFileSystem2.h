#pragma once
#include <Windows.h>
#include <unknwn.h>

// {62616943-38E6-4BBB-84D1-DAB847CB2145}
class __declspec(uuid("62616943-38E6-4BBB-84D1-DAB847CB2145")) IHutaoNativeFileSystem2 : public IUnknown
{
public:
    virtual HRESULT __stdcall CreateLink(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName) = 0;
};
