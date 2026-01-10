#pragma once
#include <Windows.h>
#include <unknwn.h>

// {A1A44E9E-858D-4746-B44E-213A1DDA4511}
class __declspec(uuid("A1A44E9E-858D-4746-B44E-213A1DDA4511")) IHutaoAString : public ::IUnknown
{
public:
    virtual HRESULT __stdcall GetBuffer(LPCSTR* buffer) = 0;
};
