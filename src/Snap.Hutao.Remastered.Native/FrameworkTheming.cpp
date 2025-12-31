#include "FrameworkTheming.h"
#include "types.h"
#include "Error.h"
#include <Windows.h>
#include <cstring>
#include <atomic>

// Global function pointers
DXamlInstanceStorageGetValueFunc pDXamlInstanceStorageGetValue = nullptr;
DXamlServicesGetHandleFunc pDXamlServicesGetHandle = nullptr;
FrameworkThemingOnThemeChangedFunc pFrameworkThemingOnThemeChanged = nullptr;

// Initialization flag
std::atomic<int32_t> g_InitializationResult(-1);

// Static initialization once flag
static INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT;

DLL_EXPORT HRESULT __stdcall FrameworkThemingSetTheme(Theme theme)
{
    // Validate theme parameter (matching C# validation)
    ThrowIfAndReturn(((static_cast<byte>(theme) & 0x03) > 0x02), "Invalid theme value", E_INVALIDARG);

    BOOL fPending = FALSE;
    HRESULT hr = S_OK;

    // One-time initialization
    if (!InitOnceBeginInitialize(&g_InitOnce, 0, &fPending, nullptr)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        ThrowForHR(hr, "InitOnceBeginInitialize failed");
        return hr;
    }

    if (fPending) {
        hr = InitializeFrameworkThemingFunctions();
        g_InitializationResult = hr;

        if (!InitOnceComplete(&g_InitOnce, 0, nullptr)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            ThrowForHR(hr, "InitOnceComplete failed");
            return hr;
        }
    }
    else {
        hr = static_cast<HRESULT>(g_InitializationResult.load());
    }

    ThrowIfAndReturn(FAILED(hr), "InitializeFrameworkThemingFunctions failed", hr);

    // Check if function pointers are initialized
    ThrowIfAndReturn(!pDXamlInstanceStorageGetValue || !pDXamlServicesGetHandle || !pFrameworkThemingOnThemeChanged,
        "FrameworkTheming functions not initialized", E_UNEXPECTED);

    // Get DXamlCore instance
    DXamlCoreAbi* pXamlCore = nullptr;
    hr = pDXamlInstanceStorageGetValue(&pXamlCore);
    ThrowIfAndReturn(FAILED(hr), "DXamlInstanceStorageGetValue failed", hr);
    ThrowIfAndReturn(!pXamlCore, "DXamlCore instance is null", E_HANDLE);

    // Get CCoreServices from DXamlCore (offset 8 * sizeof(uint64_t) = 64 bytes)
    // In C#: CCoreServiceAbi* pCoreService = (CCoreServiceAbi*)((ulong*)pXamlCore + 8);
    CCoreServiceAbi* pCoreService = reinterpret_cast<CCoreServiceAbi*>(
        reinterpret_cast<uint64_t*>(pXamlCore) + 8);

    // Get FrameworkTheming instance from CCoreServices (offset 0x670)
    // In C#: FrameworkThemingAbi* theming = *(FrameworkThemingAbi**)(*(ulong*)pCoreService + 0x670L);
    uint64_t* pCoreServiceVtable = reinterpret_cast<uint64_t*>(pCoreService);
    uint64_t coreServiceThis = *pCoreServiceVtable;
    FrameworkThemingAbi* theming = *reinterpret_cast<FrameworkThemingAbi**>(coreServiceThis + 0x670);

    ThrowIfAndReturn(!theming, "FrameworkTheming instance not found", E_HANDLE);

    // Set theme value (offset 0x50)
    // In C#: ((Theme*)theming)[0x50] = theme;
    Theme* themePtr = reinterpret_cast<Theme*>(theming);
    themePtr[0x50] = theme;

    // Call OnThemeChanged with forceUpdate = true
    hr = pFrameworkThemingOnThemeChanged(theming, TRUE);
    ThrowIfAndReturn(FAILED(hr), "FrameworkTheming::OnThemeChanged failed", hr);
    return hr;
}

HRESULT FrameworkThemingInitialize()
{
    BOOL fPending = FALSE;
    HRESULT hr = S_OK;

    if (!InitOnceBeginInitialize(&g_InitOnce, 0, &fPending, nullptr)) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        ThrowForHR(hr, "InitOnceBeginInitialize failed");
        return hr;
    }

    if (fPending) {
        hr = InitializeFrameworkThemingFunctions();
        g_InitializationResult = hr;

        if (!InitOnceComplete(&g_InitOnce, 0, nullptr)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            ThrowForHR(hr, "InitOnceComplete failed");
            return hr;
        }
        return hr;
    }

    return static_cast<HRESULT>(g_InitializationResult.load());
}

BOOL FrameworkThemingIsInitialized()
{
    return g_InitializationResult.load() >= 0;
}

HMODULE GetMuxModule()
{
    HMODULE hModule = GetModuleHandleW(L"microsoft.ui.xaml.dll");
    if (!hModule) {
        hModule = LoadLibraryW(L"microsoft.ui.xaml.dll");
    }
    return hModule;
}

DWORD GetImageSize(HMODULE hModule)
{
    if (!hModule) {
        return 0;
    }

    PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return 0;
    }

    PIMAGE_NT_HEADERS64 pNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(
        reinterpret_cast<uint8_t*>(hModule) + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return 0;
    }

    return pNtHeaders->OptionalHeader.SizeOfImage;
}

const uint8_t* FindPattern(const uint8_t* haystack, size_t haystackSize, 
                          const uint8_t* needle, size_t needleSize)
{
    if (!haystack || !needle || needleSize == 0 || haystackSize < needleSize) {
        return nullptr;
    }

    for (size_t i = 0; i <= haystackSize - needleSize; ++i) {
        if (memcmp(haystack + i, needle, needleSize) == 0) {
            return haystack + i;
        }
    }

    return nullptr;
}

HRESULT InitializeFrameworkThemingFunctions()
{
    HMODULE mux = GetMuxModule();
    ThrowIfAndReturn(!mux, "microsoft.ui.xaml.dll not found", HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND));

    DWORD imageSize = GetImageSize(mux);
    ThrowIfAndReturn(imageSize == 0, "GetImageSize failed", E_FAIL);

    const uint8_t* moduleBase = reinterpret_cast<const uint8_t*>(mux);
    const uint8_t* moduleEnd = moduleBase + imageSize;

    // Pattern for DXamlInstanceStorage::GetValue
    // 40 53                    push    rbx
    // 48 83 EC 20              sub     rsp, 20h
    // 48 8B D9                 mov     rbx, phValue
    // 8B 0D (?? ?? 6B 00)      mov     ecx, cs:?g_dwTlsIndex@DXamlInstanceStorage@@3KA ; dwTlsIndex
    static const uint8_t patternDXamlInstanceStorageGetValue[] = {
        0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0xD9, 0x8B, 0x0D
    };

    const uint8_t* foundGetValue = FindPattern(moduleBase, imageSize,
        patternDXamlInstanceStorageGetValue, sizeof(patternDXamlInstanceStorageGetValue));
    ThrowIfAndReturn(!foundGetValue, "DXamlInstanceStorage::GetValue pattern not found", E_NOT_FOUND);

    pDXamlInstanceStorageGetValue = reinterpret_cast<DXamlInstanceStorageGetValueFunc>(
        const_cast<uint8_t*>(foundGetValue));

    // Pattern for DXamlServices::GetHandle
    // First pattern: 48 83 EC 28              sub     rsp, 28h
    //                48 83 64 24 30 00        and     [rsp+28h+arg_0], 0
    //                48 8D 4C 24 30           lea     rcx, [rsp+28h+arg_0]
    static const uint8_t patternDXamlServicesGetHandle[] = {
        0x48, 0x83, 0xEC, 0x28, 0x48, 0x83, 0x64, 0x24, 0x30, 0x00, 0x48, 0x8D, 0x4C, 0x24, 0x30
    };

    // Second pattern for verification
    // 48 8B 44 24 30           mov     rax, [rsp+28h+arg_0]
    // 48 8B 40 40              mov     rax, [rax+40h]
    // 48 83 C4 28              add     rsp, 28h
    // C3                       retn
    static const uint8_t secondPatternDXamlServicesGetHandle[] = {
        0x48, 0x8B, 0x44, 0x24, 0x30, 0x48, 0x8B, 0x40, 0x40, 0x48, 0x83, 0xC4, 0x28, 0xC3
    };

    // Pattern to exclude (InternalDebugInterop::GetCore)
    // 48 89 4C 24 08           mov     [rsp+arg_0], rcx
    static const uint8_t patternInternalDebugInteropGetCore[] = {
        0x48, 0x89, 0x4C, 0x24, 0x08
    };

    // Search for DXamlServicesGetHandle pattern
    size_t offset = 0;
    const uint8_t* foundGetHandle = nullptr;
    
    while (offset < imageSize) {
        foundGetHandle = FindPattern(moduleBase + offset, imageSize - offset,
            patternDXamlServicesGetHandle, sizeof(patternDXamlServicesGetHandle));
        
        if (!foundGetHandle) {
            break;
        }

        size_t foundOffset = foundGetHandle - moduleBase;
        
        // Check if this is NOT InternalDebugInterop::GetCore
        // (pattern should NOT be present 5 bytes before)
        if (foundOffset >= 5) {
            const uint8_t* checkStart = foundGetHandle - 5;
            if (memcmp(checkStart, patternInternalDebugInteropGetCore, 
                      sizeof(patternInternalDebugInteropGetCore)) != 0) {
                
                // Check if second pattern follows
                const uint8_t* secondPatternStart = foundGetHandle + sizeof(patternDXamlServicesGetHandle);
                if (secondPatternStart + sizeof(secondPatternDXamlServicesGetHandle) <= moduleEnd) {
                    if (memcmp(secondPatternStart, secondPatternDXamlServicesGetHandle,
                              sizeof(secondPatternDXamlServicesGetHandle)) == 0) {
                        // Found the correct function
                        break;
                    }
                }
            }
        }

        offset = foundOffset + 1;
        foundGetHandle = nullptr;
    }

    ThrowIfAndReturn(!foundGetHandle, "DXamlServices::GetHandle pattern not found", E_NOT_FOUND);

    pDXamlServicesGetHandle = reinterpret_cast<DXamlServicesGetHandleFunc>(
        const_cast<uint8_t*>(foundGetHandle));

    // Pattern for FrameworkTheming::OnThemeChanged
    // 48 89 5C 24 10           mov     [rsp+arg_8], rbx
    // 48 89 6C 24 18           mov     [rsp+arg_10], rbp
    // 48 89 74 24 20           mov     [rsp+arg_18], rsi
    // 57                       push    rdi
    // 41 56                    push    r14
    // 41 57                    push    r15
    // 48 83 EC 40              sub     rsp, 40h
    // 48 8B 05 ?? ?? 3D 00     mov     rax, cs:__security_cookie
    static const uint8_t patternFrameworkThemingOnThemeChanged[] = {
        0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x6C, 0x24, 0x18, 0x48, 0x89, 0x74, 0x24, 0x20,
        0x57, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x40, 0x48, 0x8B, 0x05
    };

    // Second pattern for verification: 0x3D, 0x00
    static const uint8_t secondPatternFrameworkThemingOnThemeChanged[] = {
        0x3D, 0x00
    };

    // Search for FrameworkThemingOnThemeChanged pattern
    offset = 0;
    const uint8_t* foundOnThemeChanged = nullptr;
    
    while (offset < imageSize) {
        foundOnThemeChanged = FindPattern(moduleBase + offset, imageSize - offset,
            patternFrameworkThemingOnThemeChanged, sizeof(patternFrameworkThemingOnThemeChanged));
        
        if (!foundOnThemeChanged) {
            break;
        }

        size_t foundOffset = foundOnThemeChanged - moduleBase;
        
        // Check if second pattern follows (2 bytes after the first pattern)
        const uint8_t* secondPatternStart = foundOnThemeChanged + sizeof(patternFrameworkThemingOnThemeChanged) + 2;
        if (secondPatternStart + sizeof(secondPatternFrameworkThemingOnThemeChanged) <= moduleEnd) {
            if (memcmp(secondPatternStart, secondPatternFrameworkThemingOnThemeChanged,
                      sizeof(secondPatternFrameworkThemingOnThemeChanged)) == 0) {
                // Found the correct function
                break;
            }
        }

        offset = foundOffset + 1;
        foundOnThemeChanged = nullptr;
    }

    ThrowIfAndReturn(!foundOnThemeChanged, "FrameworkTheming::OnThemeChanged pattern not found", E_NOT_FOUND);

    pFrameworkThemingOnThemeChanged = reinterpret_cast<FrameworkThemingOnThemeChangedFunc>(
        const_cast<uint8_t*>(foundOnThemeChanged));

    return S_OK;
}
