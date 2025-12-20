#pragma once

#include "HutaoNativeLoggingCallback.h"
#include "HutaoNativeMessageCallback.h"
#include "FailureInfo.h"
#include <Windows.h>
#include <wchar.h>

#define LogInfoEx(msg, p_hr) \
    do { \
        FailureInfo info = {}; \
        info.type = FailureType::Log; \
        info.flags = FailureFlags::None; \
        info.hr = p_hr; \
        info.status = 0; \
        info.failureId = 0; \
        info.pszMessage = msg; \
        info.threadId = (intptr_t)GetCurrentThreadId(); \
        info.pszCode = __FUNCTION__; \
        info.pszFunction = __FUNCTION__; \
        info.pszFile = __FILE__; \
        info.ulineNumber = (intptr_t)__LINE__; \
        info.cFailureCount = 0; \
        info.pszCallContext = __FUNCTION__; \
        info.callContextOriginating = { 0, __FUNCTION__, nullptr }; \
        info.callContextCurrent = { 0, __FUNCTION__, nullptr }; \
        info.pszModule = "Snap.Hutao.Remastered.Native.dll"; \
        info.returnAddress = (intptr_t)_ReturnAddress(); \
        info.callerReturnAddress = (intptr_t)_ReturnAddress(); \
        g_wilCallbacksManager.InvokeLoggingCallback(&info); \
    } while(0)
#define LogInfo(msg) LogInfoEx(msg, S_OK)
#define LogMsgEx(msg, p_hr) \
    do { \
        FailureInfo info = {}; \
        info.type = FailureType::Log; \
        info.flags = FailureFlags::None; \
        info.hr = p_hr; \
        info.status = 0; \
        info.failureId = 0; \
        info.pszMessage = (PCWSTR)(msg); \
        info.threadId = (intptr_t)GetCurrentThreadId(); \
        info.pszCode = __FUNCTION__; \
        info.pszFunction = __FUNCTION__; \
        info.pszFile = __FILE__; \
        info.ulineNumber = (intptr_t)__LINE__; \
        info.cFailureCount = 0; \
        info.pszCallContext = __FUNCTION__; \
        info.callContextOriginating = { 0, __FUNCTION__, nullptr }; \
        info.callContextCurrent = { 0, __FUNCTION__, nullptr }; \
        info.pszModule = "Snap.Hutao.Remastered.Native.dll"; \
        info.returnAddress = (intptr_t)_ReturnAddress(); \
        info.callerReturnAddress = (intptr_t)_ReturnAddress(); \
        PWSTR _msgPtr = (PWSTR)info.pszMessage; \
        g_wilCallbacksManager.InvokeMessageCallback(&info, _msgPtr, 0); \
    } while(0)
#define LogMsg(msg) LogMsgEx(msg, S_OK)
#define LogException(msg) \
    do { \
        FailureInfo info = {}; \
        info.type = FailureType::Exception; \
        info.flags = FailureFlags::None; \
        info.hr = E_FAIL; \
        info.status = 0; \
        info.failureId = 0; \
        info.pszMessage = (PCWSTR)(msg); \
        info.threadId = (intptr_t)GetCurrentThreadId(); \
        info.pszCode = __FUNCTION__; \
        info.pszFunction = __FUNCTION__; \
        info.pszFile = __FILE__; \
        info.ulineNumber = (intptr_t)__LINE__; \
        info.cFailureCount = 1; \
        info.pszCallContext = __FUNCTION__; \
        info.callContextOriginating = { 0, __FUNCTION__, nullptr }; \
        info.callContextCurrent = { 0, __FUNCTION__, nullptr }; \
        info.pszModule = "Snap.Hutao.Remastered.Native.dll"; \
        info.returnAddress = (intptr_t)_ReturnAddress(); \
        info.callerReturnAddress = (intptr_t)_ReturnAddress(); \
        g_wilCallbacksManager.InvokeLoggingCallback(&info); \
    } while(0)


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
    long failureId_;
};

extern WilCallbacksManager g_wilCallbacksManager;
