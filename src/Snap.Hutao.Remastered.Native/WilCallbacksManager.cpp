#include "WilCallbacksManager.h"
#include "HutaoNativeLoggingCallback.h"
#include "HutaoNativeMessageCallback.h"
#include "FailureInfo.h"

WilCallbacksManager::WilCallbacksManager()
{

}

WilCallbacksManager::WilCallbacksManager(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback)
{

}

WilCallbacksManager::~WilCallbacksManager()
{

}

void WilCallbacksManager::Initialize(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback)
{

}

HutaoNativeLoggingCallback WilCallbacksManager::GetLoggingCallback()
{
	return loggingCallback_;
}

HutaoNativeMessageCallback WilCallbacksManager::GetMessageCallback()
{
	return messageCallback_;
}

bool WilCallbacksManager::InvokeLoggingCallback(FailureInfo* info)
{
	if (loggingCallback_.value != nullptr)
	{
		loggingCallback_.value(info);
		return true;
	}

	return false;
}

bool WilCallbacksManager::InvokeMessageCallback(FailureInfo* info, PWSTR pszDebugMessage, ULONG64 cchDebugMessage)
{
	if (messageCallback_.value != nullptr)
	{
		messageCallback_.value(info, pszDebugMessage, cchDebugMessage);
		return true;
	}
	return false;
}

void WilCallbacksManager::ClearCallbacks()
{
	loggingCallback_ = {};
	messageCallback_ = {};
}

WilCallbacksManager g_wilCallbacksManager = WilCallbacksManager();
