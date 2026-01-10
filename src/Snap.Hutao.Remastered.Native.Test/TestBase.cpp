#include "pch.h"
#include "TestBase.h"
#include <time.h>
#include <stdlib.h>

// Logging callback implementation
void __stdcall HutaoNativeLoggingCallbackImpl(FailureInfo* info)
{
    if (info)
    {
        wprintf(L"[LoggingCallback] Type: %d, Message: %s\n", 
                static_cast<int>(info->type), 
                info->pszMessage ? info->pszMessage : L"(null)");
    }
    else
    {
        wprintf(L"[LoggingCallback] Received null FailureInfo\n");
    }
}

// Message callback implementation
void __stdcall HutaoNativeMessageCallbackImpl(FailureInfo* info, PWSTR pszDebugMessage, ULONG64 cchDebugMessage)
{
    if (info)
    {
        wprintf(L"[MessageCallback] Type: %d, Message: %s\n", 
                static_cast<int>(info->type), 
                info->pszMessage ? info->pszMessage : L"(null)");
    }
    else
    {
        wprintf(L"[MessageCallback] Received null FailureInfo\n");
    }
    
    if (pszDebugMessage && cchDebugMessage > 0)
    {
        wprintf(L"[MessageCallback] Debug Message: %.*s\n", 
                (int)cchDebugMessage, 
                pszDebugMessage);
    }
}

// Test initialization function
void InitializeTestCallbacks()
{
    // Initialize test environment
    TestEnvironment::Initialize();
    
    // Set callback functions - using struct to wrap function pointers
    HutaoNativeLoggingCallback loggingCallback = HutaoNativeLoggingCallback::Create(HutaoNativeLoggingCallbackImpl);
    HutaoNativeMessageCallback messageCallback = HutaoNativeMessageCallback::Create(HutaoNativeMessageCallbackImpl);
    HutaoInitializeWilCallbacks(loggingCallback, messageCallback);
    
    wprintf(L"Test callbacks and environment initialization completed.\n");
}

// ============================================
// Test helper function implementations
// ============================================

// Note: Inline functions are already defined in header files
// Here you can add non-inline helper function implementations

// Example: Helper function to create test directory
bool CreateTestDirectory(const HutaoString& path)
{
    if (CreateDirectoryW(path.Data(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        wprintf(L"Created test directory: %s\n", path.Data());
        return true;
    }
    else
    {
        wprintf(L"Failed to create test directory: %s, error code: %d\n", path.Data(), GetLastError());
        return false;
    }
}

// Example: Helper function to delete test file
bool DeleteTestFile(const HutaoString& path)
{
    if (DeleteFileW(path.Data()) || GetLastError() == ERROR_FILE_NOT_FOUND)
    {
        wprintf(L"Deleted test file: %s\n", path.Data());
        return true;
    }
    else
    {
        wprintf(L"Failed to delete test file: %s, error code: %d\n", path.Data(), GetLastError());
        return false;
    }
}

// Example: Helper function to verify file existence
bool VerifyFileExists(const HutaoString& path)
{
    DWORD attributes = GetFileAttributesW(path.Data());
    if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        wprintf(L"File exists: %s\n", path.Data());
        return true;
    }
    else
    {
        wprintf(L"File does not exist: %s\n", path.Data());
        return false;
    }
}

// Example: Helper function to get file size
uint64_t GetFileSizeHelper(const HutaoString& path)
{
    HANDLE hFile = CreateFileW(path.Data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        wprintf(L"Cannot open file to get size: %s, error code: %d\n", path.Data(), GetLastError());
        return 0;
    }
    
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        wprintf(L"Failed to get file size: %s, error code: %d\n", path.Data(), GetLastError());
        CloseHandle(hFile);
        return 0;
    }
    
    CloseHandle(hFile);
    return static_cast<uint64_t>(fileSize.QuadPart);
}
