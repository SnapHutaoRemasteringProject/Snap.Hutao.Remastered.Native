#pragma once
#include <Windows.h>
#include <unknwn.h>

// {D8E6C944-C964-4909-8BFF-901C40670DFF}
class __declspec(uuid("D8E6C944-C964-4909-8BFF-901C40670DFF")) IHutaoNativeLogicalDrive : public IUnknown
{
public:
    virtual HRESULT __stdcall GetDiskFreeSpace(PCWSTR path, long* bytes) = 0;
};
