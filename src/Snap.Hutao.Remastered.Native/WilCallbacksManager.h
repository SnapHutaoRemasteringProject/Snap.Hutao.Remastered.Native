#pragma once

#include "HutaoNativeLoggingCallback.h"
#include "HutaoNativeMessageCallback.h"
#include "FailureInfo.h"

class WilCallbacksManager
{
public:
	WilCallbacksManager();
	WilCallbacksManager(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback);
	~WilCallbacksManager();

	void Initialize(HutaoNativeLoggingCallback loggingCallback, HutaoNativeMessageCallback messageCallback);
	HutaoNativeLoggingCallback GetLoggingCallback();
	HutaoNativeMessageCallback GetMessageCallback();
	bool InvokeLoggingCallback(FailureInfo* info);
	bool InvokeMessageCallback(FailureInfo* info, PWSTR pszDebugMessage, ULONG64 cchDebugMessage);
	void ClearCallbacks();
private:
	HutaoNativeLoggingCallback loggingCallback_ = {};
	HutaoNativeMessageCallback messageCallback_ = {};
};

extern WilCallbacksManager g_wilCallbacksManager;

