#pragma once

#include "pch.h"
#include "HutaoNativeImports.h"
#include "../Snap.Hutao.Remastered.Native/CustomImplements.h"

using namespace winrt;
using namespace Windows::Foundation;

void __stdcall HutaoNativeLoggingCallbackImpl(FailureInfo* info);
void __stdcall HutaoNativeMessageCallbackImpl(FailureInfo* info, PWSTR pszDebugMessage, ULONG64 cchDebugMessage);

void InitializeTestCallbacks();

inline bool CheckHR(HRESULT hr, const wchar_t* operation = nullptr)
{
    if (FAILED(hr))
    {
        if (operation)
        {
            wprintf(L"Operation failed '%s': HRESULT = 0x%08X\n", operation, hr);
        }
        else
        {
            wprintf(L"Operation failed: HRESULT = 0x%08X\n", hr);
        }
        return false;
    }
    return true;
}

inline void TestAssert(bool condition, const wchar_t* message = nullptr)
{
    if (!condition)
    {
        if (message)
        {
            wprintf(L"Test assertion failed: %s\n", message);
        }
        else
        {
            wprintf(L"Test assertion failed\n");
        }
    }
}

inline std::wstring FormatString(const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    
    int len = _vscwprintf(format, args) + 1;
    std::wstring buffer(len, L'\0');
    _vsnwprintf_s(&buffer[0], len, _TRUNCATE, format, args);
    
    va_end(args);
    
    // Remove trailing null character
    buffer.resize(wcslen(buffer.c_str()));
    return buffer;
}

class TestResults
{
private:
    int m_totalTests = 0;
    int m_passedTests = 0;
    int m_failedTests = 0;
    
public:
    void AddTest(bool passed)
    {
        m_totalTests++;
        if (passed)
        {
            m_passedTests++;
        }
        else
        {
            m_failedTests++;
        }
    }
    
    void PrintSummary() const
    {
        wprintf(L"\n=== Test Results Summary ===\n");
        wprintf(L"Total tests: %d\n", m_totalTests);
        wprintf(L"Passed tests: %d\n", m_passedTests);
        wprintf(L"Failed tests: %d\n", m_failedTests);
        wprintf(L"Pass rate: %.1f%%\n", m_totalTests > 0 ? (100.0 * m_passedTests / m_totalTests) : 0.0);
    }
    
    int GetTotalTests() const { return m_totalTests; }
    int GetPassedTests() const { return m_passedTests; }
    int GetFailedTests() const { return m_failedTests; }
    
    bool AllPassed() const { return m_failedTests == 0; }
};

// Test timer
class TestTimer
{
private:
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_frequency;
    
public:
    TestTimer()
    {
        QueryPerformanceFrequency(&m_frequency);
        Start();
    }
    
    void Start()
    {
        QueryPerformanceCounter(&m_startTime);
    }
    
    double GetElapsedSeconds() const
    {
        LARGE_INTEGER endTime;
        QueryPerformanceCounter(&endTime);
        return static_cast<double>(endTime.QuadPart - m_startTime.QuadPart) / m_frequency.QuadPart;
    }
    
    void PrintElapsed(const wchar_t* testName = nullptr) const
    {
        double elapsed = GetElapsedSeconds();
        if (testName)
        {
            wprintf(L"Test '%s' elapsed: %.3f seconds\n", testName, elapsed);
        }
        else
        {
            wprintf(L"Test elapsed: %.3f seconds\n", elapsed);
        }
    }
};

class TestDataGenerator
{
public:
    static std::wstring GenerateRandomString(size_t length)
    {
        static const wchar_t charset[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static const size_t charsetSize = sizeof(charset) / sizeof(charset[0]) - 1;
        
        std::wstring result;
        result.reserve(length);
        
        for (size_t i = 0; i < length; ++i)
        {
            result.push_back(charset[rand() % charsetSize]);
        }
        
        return result;
    }
    
    static std::wstring GenerateTestFilePath()
    {
        wchar_t tempPath[MAX_PATH];
        GetTempPathW(MAX_PATH, tempPath);
        
        std::wstring fileName = L"hutao_test_" + GenerateRandomString(8) + L".tmp";
        return std::wstring(tempPath) + fileName;
    }
};

class TestEnvironment
{
public:
    static void Initialize()
    {
        // Initialize random seed
        srand(static_cast<unsigned>(time(nullptr)));
        
        // Set console output encoding to UTF-8
        SetConsoleOutputCP(CP_UTF8);
        
        wprintf(L"Test environment initialized\n");
    }
    
    static void Cleanup()
    {
        wprintf(L"Test environment cleaned up\n");
    }
};
