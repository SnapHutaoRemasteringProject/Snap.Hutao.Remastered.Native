#include "dllmain.h"
#include "HutaoNativeExports.h"
#include "WilCallbacksManager.h"
#include "HotKeyCallbackManager.h"
#include "Types.h"
#include "HutaoNative.h"
#include "HutaoString.h"
#include "HutaoNativeRegistryNotification.h"
#include "HutaoNativeLoggingCallback.h"
#include "HutaoNativeMessageCallback.h"
#include "IHutaoNativeRegistryNotification_h.h"
#include "IHutaoNative_h.h"
#include "IHutaoString_h.h"
#include "CustomImplements.h"
#include "Error.h"
#include <Windows.h>

DLL_EXPORT HRESULT __stdcall HutaoCreateInstance(
    IHutaoNative** ppInstance) {

    AssertNonNullAndReturn(ppInstance);

    hutao::com_ptr<IHutaoNative> instance = hutao::make_com_ptr<HutaoNative>();
    *ppInstance = instance.detach();

    return S_OK;
}

DLL_EXPORT HRESULT __stdcall HutaoStringCreateInstance(
    IHutaoString** ppInstance) {

    AssertNonNullAndReturn(ppInstance);

    hutao::com_ptr<IHutaoString> instance = hutao::make_com_ptr<HutaoString>();
    *ppInstance = instance.detach();

    return S_OK;
}

DLL_EXPORT HRESULT __stdcall HutaoNativeRegistryNotificationCreateInstance(IHutaoNativeRegistryNotification** ppInstance)
{
    AssertNonNullAndReturn(ppInstance);

    hutao::com_ptr<IHutaoNativeRegistryNotification> instance = hutao::make_com_ptr<HutaoNativeRegistryNotification>();
    *ppInstance = instance.detach();

    return S_OK;
}

DLL_EXPORT HRESULT HutaoInitializeWilCallbacks(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback)
{
	g_wilCallbacksManager.Initialize(loggingCallback, messageCallback);
    return S_OK;
}

DLL_EXPORT void HutaoTestWilCallbacks()
{
	LogForHR(S_OK);
	LogMessage("This is a test message from HutaoTestWilCallbacks.");
}

DLL_EXPORT BOOL HutaoHResultIsWin32(HRESULT hr, WIN32_ERROR error)
{
    return hr == HRESULT_FROM_WIN32(error) ? TRUE : FALSE;
}

DLL_EXPORT HRESULT HutaoNativeHotKeyInitializeBeforeSwitchCallback(HutaoNativeHotKeyBeforeSwitchCallback callback)
{
	g_hotKeyCallbackManager.Initialize(callback);
    return S_OK;
}
