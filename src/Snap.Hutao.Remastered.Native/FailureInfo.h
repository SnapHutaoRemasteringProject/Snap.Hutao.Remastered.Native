#pragma once

#include "Types.h"
#include <Windows.h>
#include <cstdint>
#include <bcrypt.h>


enum FailureType
{
    // THROW_...
    Exception,

    // RETURN_..._LOG or RETURN_..._MSG
    Return,

    // LOG_...
    Log,

    // FAIL_FAST_...
    FailFast,
};

enum FailureFlags
{
    None = 0x00,
    RequestFailFast = 0x01,
    RequestSuppressTelemetry = 0x02,
    RequestDebugBreak = 0x04,
    NtStatus = 0x08,
};

struct CallContextInfo
{
    // incrementing ID for this call context (unique across an individual module load within process)
    long contextId;

    // the explicit name given to this context
    PCSTR contextName;

    // [optional] Message that can be associated with the call context
    PCWSTR contextMessage;
};

struct FailureInfo
{
    FailureType type;
    FailureFlags flags;
    HRESULT hr;
    NTSTATUS status;

    // incrementing ID for this specific failure (unique across an individual module load within process)
    long failureId;

    // Message is only present for _MSG logging (it's the Sprintf message)
    PCWSTR pszMessage;

    // the thread this failure was originally encountered on
    intptr_t threadId;

    // [debug only] Capture code from the macro
    PCSTR pszCode;

    // [debug only] The function name
    PCSTR pszFunction;
    PCSTR pszFile;
    intptr_t ulineNumber;

    // How many failures of 'type' have been reported in this module so far
    int cFailureCount;

    // General breakdown of the call context stack that generated this failure
    PCSTR pszCallContext;

    // The outermost (first seen) call context
    CallContextInfo callContextOriginating;

    // The most recently seen call context
    CallContextInfo callContextCurrent;

    // The module where the failure originated
    PCSTR pszModule;

    // The return address to the point that called the macro
    intptr_t returnAddress;

    // The return address of the function that includes the macro
    intptr_t callerReturnAddress;
};