#pragma once

#include "dllmain.h"
#include "Theme.h"
#include <Windows.h>
#include <cstdint>
#include <atomic>

// Forward declarations for internal types
struct CCoreServiceAbi;
struct DXamlCoreAbi;
struct FrameworkThemingAbi;

// Function pointer types matching the C# signatures
typedef HRESULT (__stdcall* DXamlInstanceStorageGetValueFunc)(DXamlCoreAbi** ppXamlCore);
typedef CCoreServiceAbi* (__stdcall* DXamlServicesGetHandleFunc)();
typedef HRESULT (__stdcall* FrameworkThemingOnThemeChangedFunc)(FrameworkThemingAbi* theming, BOOL forceUpdate);

// Global function pointers (initialized in static initialization)
extern DXamlInstanceStorageGetValueFunc pDXamlInstanceStorageGetValue;
extern DXamlServicesGetHandleFunc pDXamlServicesGetHandle;
extern FrameworkThemingOnThemeChangedFunc pFrameworkThemingOnThemeChanged;

// Initialization flag
extern std::atomic<int32_t> g_InitializationResult;

// Main export function
DLL_EXPORT HRESULT __stdcall FrameworkThemingSetTheme(Theme theme);

// Helper functions
HRESULT FrameworkThemingInitialize();
BOOL FrameworkThemingIsInitialized();

// Internal functions
HRESULT InitializeFrameworkThemingFunctions();
HMODULE GetMuxModule();
DWORD GetImageSize(HMODULE hModule);
const uint8_t* FindPattern(const uint8_t* haystack, size_t haystackSize, const uint8_t* needle, size_t needleSize);
