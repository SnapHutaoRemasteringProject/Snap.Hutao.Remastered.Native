#include "HutaoNativeRegistryNotification.h"
#include "types.h"

HutaoNativeRegistryNotification::HutaoNativeRegistryNotification()
{

}

HutaoNativeRegistryNotification::~HutaoNativeRegistryNotification()
{

}

HRESULT STDMETHODCALLTYPE HutaoNativeRegistryNotification::Start(nint callback, INT64 userData) {
    if (!callback)
    {
        return E_INVALIDARG;
    }
    
    userData_ = userData;

    return E_NOTIMPL;
}

