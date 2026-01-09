#pragma once

#include <unknwn.h>

class __declspec(uuid("A0AD4485-702B-44B6-B48E-F82240EBBAEF"))
IHutaoNativeWindowNonRude : public IUnknown
{
public:
    virtual HRESULT __stdcall Attach() = 0;
    virtual HRESULT __stdcall Detach() = 0;
};
