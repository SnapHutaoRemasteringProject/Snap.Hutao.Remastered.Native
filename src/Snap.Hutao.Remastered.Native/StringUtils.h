#pragma once

#include "dllmain.h"
#include <Windows.h>

// ×Ö·û´®×ª»»º¯Êý
LPSTR WideCharToAnsi(LPCWSTR wideStr);
LPWSTR AnsiToWideChar(LPCSTR ansiStr);
LPSTR WideCharToUtf8(LPCWSTR wideStr);
LPWSTR Utf8ToWideChar(LPCSTR utf8Str);

// ×Ö·û´®¸¨Öúº¯Êý
BOOL IsStringNumeric(LPCWSTR str);
DWORD StringToOrdinal(LPCWSTR str);
LPCSTR GetAnsiFunctionName(LPCWSTR wideFunctionName);

// ÄÚ´æ¹ÜÀí
void FreeConvertedString(LPVOID str);
