#pragma once
#include <Windows.h>

typedef unsigned char byte;
typedef unsigned int WIN32_ERROR;
typedef long long nint;
typedef unsigned long uint;

typedef nint GCHandle;

#define E_NOT_FOUND 0x80070490

// 从Types.idl合并的类型定义
typedef const wchar_t* LPCWSTR;
typedef LPCWSTR PCWSTR;

typedef struct HutaoPrivateWindowsVersion
{
    UINT major;
    UINT minor;
    UINT build;
    UINT revision;
} HutaoPrivateWindowsVersion;

typedef enum HutaoNativeHotKeyActionKind
{
    None = 0,
    MouseClickRepeatForever = 1,
    KeyPressRepeatForever = 2
} HutaoNativeHotKeyActionKind;

typedef struct HutaoNativeProcessStartInfo
{
    PCWSTR ApplicationName;
    PCWSTR CommandLine;
    BOOL InheritHandles;
    long CreationFlags; // PROCESS_CREATION_FLAGS
    PCWSTR CurrentDirectory;
} HutaoNativeProcessStartInfo;
