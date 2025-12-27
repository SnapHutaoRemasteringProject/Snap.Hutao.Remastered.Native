#include "HotKeyCallbackManager.h"
#include "HutaoNativeHotKeyBeforeSwitchCallback.h"

HotKeyCallbackManager::HotKeyCallbackManager()
{
}

HotKeyCallbackManager::~HotKeyCallbackManager()
{
}

void HotKeyCallbackManager::Initialize(HutaoNativeHotKeyBeforeSwitchCallback callback)
{
	callback_ = callback;
}

HutaoNativeHotKeyBeforeSwitchCallback HotKeyCallbackManager::GetCallback()
{
	return callback_;
}

BOOL HotKeyCallbackManager::InvokeCallback()
{
	if (callback_.value != nullptr)
	{
		return callback_.value();
	}
	return TRUE; // 如果没有回调，默认允许切换
}

void HotKeyCallbackManager::ClearCallback()
{
	callback_ = {};
}

HotKeyCallbackManager g_hotKeyCallbackManager = HotKeyCallbackManager();
