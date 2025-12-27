#pragma once

#include "HutaoNativeHotKeyBeforeSwitchCallback.h"
#include <Windows.h>

class HotKeyCallbackManager
{
public:
	HotKeyCallbackManager();
	~HotKeyCallbackManager();

	void Initialize(HutaoNativeHotKeyBeforeSwitchCallback callback);
	HutaoNativeHotKeyBeforeSwitchCallback GetCallback();
	BOOL InvokeCallback();
	void ClearCallback();

private:
	HutaoNativeHotKeyBeforeSwitchCallback callback_ = {};
};

extern HotKeyCallbackManager g_hotKeyCallbackManager;
