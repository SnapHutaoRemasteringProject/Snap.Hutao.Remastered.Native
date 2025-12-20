#include"FrameworkTheming.h"
#include "types.h"
#include "dllmain.h"
#include "Theme.h"
#include <Windows.h>
#include <cstdarg>
#include <cstdint>
#include <string.h>

DLL_EXPORT HRESULT __stdcall FrameworkThemingSetTheme(Theme theme) {
    // 验证输入参数 (匹配C#的Theme枚举)
    if ((((byte)theme) & 0x03) > 0x02) {  // BaseMask检查
        return E_INVALIDARG;
    }

    BOOL fPending = FALSE;
    HRESULT hr = S_OK;

    if (!InitOnceBeginInitialize(&g_InitOnce, 0, &fPending, nullptr)) {
        return E_FAIL;
    }
    if (fPending) {
        hr = InitializeXamlFunctions();
        g_InitializationResult = hr;

        if (!InitOnceComplete(&g_InitOnce, 0, nullptr)) {
            return E_FAIL;
        }
    }
    else {
        hr = g_InitializationResult.load();
    }

    if (SUCCEEDED(hr)) {  // 成功
        if (!g_pGetValueFunction || !g_pOnThemeChangedFunction) {
            return E_UNEXPECTED;
        }

        int64_t contextObject = 0;

        // 调用获取的 GetValue 函数
        auto getValueFunc = reinterpret_cast<HRESULT(__stdcall*)(int64_t*)>(g_pGetValueFunction);
        getValueFunc(&contextObject);

        if (contextObject == 0) {
            return E_HANDLE;
        }

        // 导航到主题对象 (根据逆向分析的结构偏移)
        int64_t* ptr1 = reinterpret_cast<int64_t*>(contextObject + 64);
        if (ptr1 == nullptr || *ptr1 == 0) {
            return E_HANDLE;
        }

        int64_t* ptr2 = reinterpret_cast<int64_t*>(*ptr1 + 1648);
        if (ptr2 == nullptr || *ptr2 == 0) {
            return E_HANDLE;
        }

        // 设置主题值
        uint8_t* themeValue = reinterpret_cast<uint8_t*>(*ptr2 + 80);
        *themeValue = (byte)theme;

        // 调用 OnThemeChanged 函数
        auto onThemeChangedFunc = reinterpret_cast<HRESULT(__stdcall*)(int64_t, uint8_t)>(g_pOnThemeChangedFunction);
        return onThemeChangedFunc(*ptr2, 1);  // forceUpdate = true

    }
    else {  // 初始化失败
        return hr;
    }
}

static HRESULT InitializeXamlFunctions() {
    HMODULE xamlModule = GetModuleHandleW(L"microsoft.ui.xaml.dll");
    if (!xamlModule) {
        xamlModule = LoadLibraryW(L"microsoft.ui.xaml.dll");
        if (!xamlModule) {
            return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
        }
    }

    HRESULT hr = InitializeXamlFunctions_Exact(xamlModule);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

static HRESULT InitializeXamlFunctions_Exact(HMODULE xamlModule) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)xamlModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((uint8_t*)xamlModule + dosHeader->e_lfanew);
    uint8_t* moduleBase = (uint8_t*)xamlModule;
    uint8_t* moduleEnd = moduleBase + ntHeaders->OptionalHeader.SizeOfImage;

    // 准备特征码1（11字节）
    GetValuePattern getValuePattern;
    getValuePattern.part1 = -2092412096;
    getValuePattern.part2 = -1958207252;
    getValuePattern.part3 = -29735;
    getValuePattern.part4 = 13;

    // 搜索特征码1
    void* foundGetValue = FindPattern(moduleBase, ntHeaders->OptionalHeader.SizeOfImage,
        &getValuePattern, sizeof(getValuePattern));

    if (!foundGetValue) {
        // 原始代码调用 LogFile 两次
        return E_NOT_FOUND;
    }

    g_pGetValueFunction = foundGetValue;

    // 准备特征码2（26字节）
    OnThemeChangedPattern onThemeChangedPattern;
    onThemeChangedPattern.parts[0] = 610044232;
    onThemeChangedPattern.parts[1] = 1820936208;
    onThemeChangedPattern.parts[2] = -1991763932;
    onThemeChangedPattern.parts[3] = 1461724276;
    onThemeChangedPattern.parts[4] = 1463899713;
    onThemeChangedPattern.parts[5] = 552371016;
    onThemeChangedPattern.lastPart = 1526;

    // 搜索特征码2（与原始循环逻辑一致）
    size_t searchOffset = 0;
    size_t moduleSize = ntHeaders->OptionalHeader.SizeOfImage;

    while (searchOffset + 26 <= moduleSize) {
        void* currentPos = moduleBase + searchOffset;
        void* found = FindPattern(currentPos, moduleSize - searchOffset,
            &onThemeChangedPattern, sizeof(onThemeChangedPattern));

        if (!found) break;

        uintptr_t offset = (uintptr_t)found - (uintptr_t)moduleBase;

        // ==== 完全照抄原始验证条件 ====
        if ((moduleSize - offset) != 30 && *((uint8_t*)found + 30) == 8) {
            g_pOnThemeChangedFunction = found;
            return S_OK; // 成功
        }

        searchOffset = offset + 31;
        if (searchOffset >= moduleSize) {
            // 原始代码记录错误日志后返回 2147943568
            return E_NOT_FOUND;
        }
    }

    return E_NOT_FOUND;
}

static void* FindPattern(void* start, size_t size, const void* pattern, size_t patternSize) {
    for (size_t i = 0; i <= size - patternSize; ++i) {
        if (memcmp((uint8_t*)start + i, pattern, patternSize) == 0) {
            return (uint8_t*)start + i;
        }
    }
    return nullptr;
}

// 辅助函数：预初始化（可选）
HRESULT FrameworkThemingInitialize() {
    BOOL fPending = FALSE;
    if (!InitOnceBeginInitialize(&g_InitOnce, 0, &fPending, nullptr)) {
        return E_FAIL;
    }

    if (fPending) {
        HRESULT hr = InitializeXamlFunctions();
        g_InitializationResult = hr;

        if (!InitOnceComplete(&g_InitOnce, 0, nullptr)) {
            return E_FAIL;
        }
        return hr;
    }

    return g_InitializationResult.load();
}

// 辅助函数：检查是否已初始化
BOOL FrameworkThemingIsInitialized() {
    return g_InitializationResult.load() >= 0;
}