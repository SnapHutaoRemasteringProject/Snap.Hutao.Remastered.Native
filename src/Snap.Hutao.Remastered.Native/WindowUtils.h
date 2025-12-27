#pragma once

#include "dllmain.h"
#include <Windows.h>

// 窗口切换函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSwitchToWindow(HWND hWnd);

// 分层窗口样式函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesAddExtendedStyleLayered(HWND hWnd);
DLL_EXPORT HRESULT __stdcall WindowUtilitiesRemoveExtendedStyleLayered(HWND hWnd);
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetLayeredWindowTransparency(HWND hWnd, BYTE opacity);

// 工具窗口样式函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesAddExtendedStyleToolWindow(HWND hWnd);

// 重叠窗口样式函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesRemoveStyleOverlappedWindow(HWND hWnd);

// 窗口缩放函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesGetRasterizationScaleForWindow(HWND hWnd, float* scale);

// 窗口启用状态函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetWindowIsEnabled(HWND hWnd, BOOL isEnabled);

// 窗口所有者函数
DLL_EXPORT HRESULT __stdcall WindowUtilitiesSetWindowOwner(HWND hWnd, HWND hWndOwner);
