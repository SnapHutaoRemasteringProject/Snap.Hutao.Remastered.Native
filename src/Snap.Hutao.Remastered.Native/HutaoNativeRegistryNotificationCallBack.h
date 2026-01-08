#pragma once
#include "types.h"

using HutaoNativeRegistryNotificationProc = void (*)(GCHandle userData);

struct HutaoNativeRegistryNotificationCallBack
{
private:
    HutaoNativeRegistryNotificationProc m_value;

public:
    HutaoNativeRegistryNotificationCallBack() = default;

    explicit HutaoNativeRegistryNotificationCallBack(HutaoNativeRegistryNotificationProc method)
        : m_value(method)
    {
    }

    static HutaoNativeRegistryNotificationCallBack Create(HutaoNativeRegistryNotificationProc method)
    {
        return HutaoNativeRegistryNotificationCallBack(method);
    }

    HutaoNativeRegistryNotificationProc value() const
    {
        return m_value;
    }

    bool has_value() const
    {
        return m_value != nullptr;
    }
};

