#pragma once
#include <Windows.h>
#include <unknwn.h>

// {8628902F-835C-4293-8580-794EC9BCCB98}
class __declspec(uuid("8628902F-835C-4293-8580-794EC9BCCB98")) IHutaoNativeInputLowLevelKeyboardSource : public IUnknown
{
public:
    virtual HRESULT __stdcall Attach(intptr_t callback) = 0;
    virtual HRESULT __stdcall Detach(intptr_t callback) = 0;
};
