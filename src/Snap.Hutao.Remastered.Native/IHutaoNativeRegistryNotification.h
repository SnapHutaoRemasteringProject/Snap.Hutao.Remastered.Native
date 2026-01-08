#pragma once

#include <unknwn.h>
#include <Windows.h>
#include "types.h"
#include "HutaoNativeRegistryNotificationCallBack.h"

class __declspec(uuid("EF118E91-8AD9-4C27-997D-DAF8910B34BE"))
IHutaoNativeRegistryNotification : public IUnknown
{
public:
    virtual HRESULT __stdcall Start(HutaoNativeRegistryNotificationCallBack callback, GCHandle userData) = 0;
};
