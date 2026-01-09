#pragma once

#include <Windows.h>
#include <intrin.h>

#define ThrowForHR(hr, message) \
    if (FAILED(hr)) \
        ThrowForHRCore(_ReturnAddress(), __LINE__, __FUNCTION__, __FILE__, hr, L##message)
#define ThrowForHRAndReturn(hr, message) \
    if (FAILED(hr)) \
    { \
        ThrowForHRCore(_ReturnAddress(), __LINE__, __FUNCTION__, __FILE__, hr, L##message); \
        return hr; \
	}
#define LogForHR(hr) \
    LogForHRCore(_ReturnAddress(), __LINE__, __FUNCTION__, __FILE__, hr)
#define LogMessageForHR(hr, message) \
    LogMessageForHRCore(_ReturnAddress(), __LINE__, __FUNCTION__, __FILE__, hr, L##message)
#define LogMessage(message) \
    LogMessageCore(_ReturnAddress(), __LINE__, __FUNCTION__, __FILE__, L##message)

#define ThrowIf(condition, message, hr) \
    if (condition) { \
        ThrowForHR(hr, message); \
    }
#define ThrowIfAndReturn(condition, message, hr) \
    if (condition) { \
        ThrowForHR(hr, message); \
        return hr; \
    }
#define AssertNonNull(ptr) \
    if ((ptr) == nullptr) { \
        ThrowForHR(E_POINTER, #ptr" can not be null"); \
    }
#define AssertNonNullAndReturn(ptr) \
    if ((ptr) == nullptr) { \
        ThrowForHR(E_POINTER, #ptr" can not be null"); \
        return E_POINTER; \
    }

void ThrowForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr, PCWSTR message);
void LogForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr);
void LogMessageForHRCore(void* returnAddress, int line, PCSTR function, PCSTR file, HRESULT hr, PCWSTR message);
void LogMessageCore(void* returnAddress, int line, PCSTR function, PCSTR file, PCWSTR message);
