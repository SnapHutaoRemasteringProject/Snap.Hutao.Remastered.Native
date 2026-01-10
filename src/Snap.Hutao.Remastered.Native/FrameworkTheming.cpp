#include "pch.h"
#include "FrameworkTheming.h"
#include <atomic>

// Global function pointers
DXamlInstanceStorageGetValueFunc pDXamlInstanceStorageGetValue = nullptr;
FrameworkThemingOnThemeChangedFunc pFrameworkThemingOnThemeChanged = nullptr;

// Initialization flag
std::atomic<int32_t> g_InitializationResult(-1);

// Static initialization once flag
static INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT;

// Helper: get module base pointer and end pointer
static inline const uint8_t* ModuleBase(HMODULE h) {
    return reinterpret_cast<const uint8_t*>(h);
}

static inline const uint8_t* ModuleEnd(const uint8_t* base, DWORD size) {
    return base + size;
}

// Helper: safe read of uint32 at offset within module image
static bool ReadUint32At(const uint8_t* base, size_t imageSize, size_t offset, uint32_t& out) {
    if (offset + sizeof(uint32_t) > imageSize) return false;
    out = *reinterpret_cast<const uint32_t*>(base + offset);
    return true;
}

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
    // Use AssertNonNullAndReturn for pointer sanity checks
    AssertNonNullAndReturn(pDXamlInstanceStorageGetValue);
    AssertNonNullAndReturn(pFrameworkThemingOnThemeChanged);

    // Get DXamlCore instance
    DXamlCoreAbi* pXamlCore = nullptr;
    hr = pDXamlInstanceStorageGetValue(&pXamlCore);
    ThrowIfAndReturn(FAILED(hr), "DXamlInstanceStorageGetValue failed", hr);
    AssertNonNullAndReturn(pXamlCore);

    // Get CCoreServices from DXamlCore (offset 8 * sizeof(uint64_t) = 64 bytes)
    CCoreServiceAbi* pCoreService = reinterpret_cast<CCoreServiceAbi*>(
        reinterpret_cast<uint64_t*>(pXamlCore) + 8);

    // Get FrameworkTheming instance from CCoreServices (offset 0x670)
    uint64_t* pCoreServiceVtable = reinterpret_cast<uint64_t*>(pCoreService);
    uint64_t coreServiceThis = *pCoreServiceVtable;
    FrameworkThemingAbi* theming = *reinterpret_cast<FrameworkThemingAbi**>(coreServiceThis + 0x670);

    ThrowIfAndReturn(!theming, "FrameworkTheming instance not found", E_HANDLE);

    // Set theme value (offset 0x50)
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

    const uint8_t* moduleBase = ModuleBase(mux);
    const uint8_t* moduleEnd = ModuleEnd(moduleBase, imageSize);

    // Patterns and constants (grouped for readability)
    static const uint8_t patternInternalDebugInteropGetCore[] = { 0x48, 0x89, 0x4C, 0x24, 0x08 };

    // FrameworkTheming::OnThemeChanged pattern (derived from reverse.Data())
    static const uint8_t patternFrameworkThemingOnThemeChangedV15[] = {
        0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x6C, 0x24, 0x18,
        0x48, 0x89, 0x74, 0x24, 0x20, 0x57, 0x41, 0x56, 0x41, 0x57,
        0x48, 0x83, 0xEC, 0x20, 0xF6, 0x05
    };

    // Locate FrameworkTheming::OnThemeChanged (reverse-style)
    size_t v6_iter = 0;
    const uint8_t* foundOnThemeChanged = nullptr;
    while ((imageSize - v6_iter) >= sizeof(patternFrameworkThemingOnThemeChangedV15)) {
        const uint8_t* candidate = FindPattern(moduleBase + v6_iter,
            imageSize - v6_iter,
            patternFrameworkThemingOnThemeChangedV15,
            sizeof(patternFrameworkThemingOnThemeChangedV15));

        if (!candidate) break;

        size_t matchOffset = candidate - moduleBase;
        if (matchOffset + 30 < imageSize) {
            if ((imageSize - matchOffset) != 30 && *(moduleBase + matchOffset + 30) == 8) {
                foundOnThemeChanged = candidate;
                break;
            }
        }

        v6_iter = matchOffset + 31;
    }

    ThrowIfAndReturn(!foundOnThemeChanged, "FrameworkTheming::OnThemeChanged pattern not found", E_NOT_FOUND);
    pFrameworkThemingOnThemeChanged = reinterpret_cast<FrameworkThemingOnThemeChangedFunc>(
        const_cast<uint8_t*>(foundOnThemeChanged));

    // Locate DXamlInstanceStorage::GetValue using reverse-derived region limits
    if (imageSize < 0x40) {
        ThrowIfAndReturn(true, "Invalid module image", E_FAIL);
    }

    uint32_t e_lfanew = 0;
    if (!ReadUint32At(moduleBase, imageSize, 0x3C, e_lfanew)) {
        ThrowIfAndReturn(true, "Failed to read module headers", E_FAIL);
    }

    uint32_t v4 = 0;
    if (!ReadUint32At(moduleBase, imageSize, static_cast<size_t>(e_lfanew) + 80, v4)) {
        ThrowIfAndReturn(true, "Failed to read module headers", E_FAIL);
    }

    static const uint8_t patternDXamlInstanceStorageGetValueV12[] = {
        0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0xD9, 0x8B, 0x0D
    };

    if (v4 < sizeof(patternDXamlInstanceStorageGetValueV12)) {
        ThrowIfAndReturn(true, "Failed to locate DXamlInstanceStorage::GetValue(void** phValue)", E_NOT_FOUND);
    }

    const uint8_t* regionBase = moduleBase;
    size_t regionSize = static_cast<size_t>(v4);

    const uint8_t* foundGetValueV12 = FindPattern(regionBase, regionSize,
        patternDXamlInstanceStorageGetValueV12, sizeof(patternDXamlInstanceStorageGetValueV12));

    ThrowIfAndReturn(!foundGetValueV12, "Failed to locate DXamlInstanceStorage::GetValue(void** phValue)", E_NOT_FOUND);

    pDXamlInstanceStorageGetValue = reinterpret_cast<DXamlInstanceStorageGetValueFunc>(
        const_cast<uint8_t*>(foundGetValueV12));

    return S_OK;
}
