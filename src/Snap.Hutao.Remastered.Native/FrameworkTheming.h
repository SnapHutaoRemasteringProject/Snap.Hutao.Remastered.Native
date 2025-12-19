#pragma once

#include"dllmain.h"
#include"Theme.h"
#include <Windows.h>
#include <cstdint>
#include <atomic>

INIT_ONCE InitOnce = INIT_ONCE{ FALSE };
BOOL fPending = false;

static INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT;
static std::atomic<int32_t> g_InitializationResult = -1;
static void* g_pGetValueFunction = nullptr;
static void* g_pOnThemeChangedFunction = nullptr;

static HRESULT InitializeXamlFunctions();
static HRESULT InitializeXamlFunctions_Exact(HMODULE xamlModule);
static void* FindPattern(void* start, size_t size, const void* pattern, size_t patternSize);

DLL_EXPORT HRESULT __stdcall FrameworkThemingSetTheme(Theme theme);

#pragma pack(push, 1)
struct GetValuePattern {
    int32_t part1; // -2092412096
    int32_t part2; // -1958207252
    int16_t part3; // -29735
    uint8_t part4; // 13
};

struct OnThemeChangedPattern {
    int32_t parts[6]; // {610044232, 1820936208, -1991763932, 1461724276, 1463899713, 552371016}
    int16_t lastPart; // 1526
};
#pragma pack(pop)