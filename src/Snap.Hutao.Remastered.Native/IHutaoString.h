#pragma once
#include <Windows.h>
#include <unknwn.h>

// {F1F44E9E-858D-4746-B44E-213A1DDA4510}
class __declspec(uuid("F1F44E9E-858D-4746-B44E-213A1DDA4510")) IHutaoString : public IUnknown
{
public:
    virtual HRESULT __stdcall GetBuffer(LPCWSTR* buffer) = 0;
};
