#include "Error.h"
#include "WilCallbacksManager.h"
#include "FailureInfo.h"

void ThrowForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr, PCWSTR message)
{
    FailureInfo info = {};
    info.type = FailureType::Exception;
    info.flags = FailureFlags::None;
    info.hr = hr;
    info.status = 0;
    info.failureId = 0;
    info.pszMessage = message;
    info.threadId = (intptr_t)GetCurrentThreadId();
    info.pszCode = function;
    info.pszFunction = function;
    info.pszFile = file;
    info.ulineNumber = (intptr_t)line;
    info.cFailureCount = 0;
    info.pszCallContext = function;
    info.callContextOriginating = { 0, function, nullptr };
    info.callContextCurrent = { 0, function, nullptr };
    info.pszModule = "Snap.Hutao.Remastered.Native.dll";
    info.returnAddress = (intptr_t)returnAddress;
    info.callerReturnAddress = (intptr_t)returnAddress;
    g_wilCallbacksManager.InvokeLoggingCallback(&info);
}

void LogForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr)
{
    FailureInfo info = {};
    info.type = FailureType::Log;
    info.flags = FailureFlags::None;
    info.hr = hr;
    info.status = 0;
    info.failureId = 0;
    info.pszMessage = L"";
    info.threadId = (intptr_t)GetCurrentThreadId();
    info.pszCode = function;
    info.pszFunction = function;
    info.pszFile = file;
    info.ulineNumber = (intptr_t)line;
    info.cFailureCount = 0;
    info.pszCallContext = function;
    info.callContextOriginating = { 0, function, nullptr };
    info.callContextCurrent = { 0, function, nullptr };
    info.pszModule = "Snap.Hutao.Remastered.Native.dll";
    info.returnAddress = (intptr_t)returnAddress;
    info.callerReturnAddress = (intptr_t)returnAddress;
    g_wilCallbacksManager.InvokeLoggingCallback(&info);
}

void LogMessageForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr, PCWSTR message)
{
    FailureInfo info = {};
    info.type = FailureType::Log;
    info.flags = FailureFlags::None;
    info.hr = hr;
    info.status = 0;
    info.failureId = 0;
    info.pszMessage = message;
    info.threadId = (intptr_t)GetCurrentThreadId();
    info.pszCode = function;
    info.pszFunction = function;
    info.pszFile = file;
    info.ulineNumber = (intptr_t)line;
    info.cFailureCount = 0;
    info.pszCallContext = function;
    info.callContextOriginating = { 0, function, nullptr };
    info.callContextCurrent = { 0, function, nullptr };
    info.pszModule = "Snap.Hutao.Remastered.Native.dll";
    info.returnAddress = (intptr_t)returnAddress;
    info.callerReturnAddress = (intptr_t)returnAddress;
    PWSTR _msgPtr = (PWSTR)info.pszMessage;
    g_wilCallbacksManager.InvokeMessageCallback(&info, _msgPtr, 0);
}

void LogMessageCore(void* returnAddress, int line, PCSTR function, PCSTR file, PCWSTR message)
{
    FailureInfo info = {};
    info.type = FailureType::Log;
    info.flags = FailureFlags::None;
    info.hr = S_OK;
    info.status = 0;
    info.failureId = 0;
    info.pszMessage = message;
    info.threadId = (intptr_t)GetCurrentThreadId();
    info.pszCode = function;
    info.pszFunction = function;
    info.pszFile = file;
    info.ulineNumber = (intptr_t)line;
    info.cFailureCount = 0;
    info.pszCallContext = function;
    info.callContextOriginating = { 0, function, nullptr };
    info.callContextCurrent = { 0, function, nullptr };
    info.pszModule = "Snap.Hutao.Remastered.Native.dll";
    info.returnAddress = (intptr_t)returnAddress;
    info.callerReturnAddress = (intptr_t)returnAddress;
    PWSTR _msgPtr = (PWSTR)info.pszMessage;
    g_wilCallbacksManager.InvokeMessageCallback(&info, _msgPtr, 0);
}
