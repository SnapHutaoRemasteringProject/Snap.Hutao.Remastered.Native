#include "StringUtils.h"

// 将宽字符字符串转换为ANSI字符串
LPSTR WideCharToAnsi(LPCWSTR wideStr)
{
    if (!wideStr) {
        return NULL;
    }

    // 获取所需的缓冲区大小
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        return NULL;
    }

    // 分配缓冲区
    LPSTR ansiStr = (LPSTR)malloc(bufferSize);
    if (!ansiStr) {
        return NULL;
    }

    // 执行转换
    if (WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ansiStr, bufferSize, NULL, NULL) == 0) {
        free(ansiStr);
        return NULL;
    }

    return ansiStr;
}

// 将ANSI字符串转换为宽字符字符串
LPWSTR AnsiToWideChar(LPCSTR ansiStr)
{
    if (!ansiStr) {
        return NULL;
    }

    // 获取所需的缓冲区大小
    int bufferSize = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, NULL, 0);
    if (bufferSize == 0) {
        return NULL;
    }

    // 分配缓冲区
    LPWSTR wideStr = (LPWSTR)malloc(bufferSize * sizeof(WCHAR));
    if (!wideStr) {
        return NULL;
    }

    // 执行转换
    if (MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, wideStr, bufferSize) == 0) {
        free(wideStr);
        return NULL;
    }

    return wideStr;
}

// 将宽字符字符串转换为UTF-8字符串
LPSTR WideCharToUtf8(LPCWSTR wideStr)
{
    if (!wideStr) {
        return NULL;
    }

    // 获取所需的缓冲区大小
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0) {
        return NULL;
    }

    // 分配缓冲区
    LPSTR utf8Str = (LPSTR)malloc(bufferSize);
    if (!utf8Str) {
        return NULL;
    }

    // 执行转换
    if (WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, bufferSize, NULL, NULL) == 0) {
        free(utf8Str);
        return NULL;
    }

    return utf8Str;
}

// 将UTF-8字符串转换为宽字符字符串
LPWSTR Utf8ToWideChar(LPCSTR utf8Str)
{
    if (!utf8Str) {
        return NULL;
    }

    // 获取所需的缓冲区大小
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    if (bufferSize == 0) {
        return NULL;
    }

    // 分配缓冲区
    LPWSTR wideStr = (LPWSTR)malloc(bufferSize * sizeof(WCHAR));
    if (!wideStr) {
        return NULL;
    }

    // 执行转换
    if (MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideStr, bufferSize) == 0) {
        free(wideStr);
        return NULL;
    }

    return wideStr;
}

// 检查字符串是否为纯数字
BOOL IsStringNumeric(LPCWSTR str)
{
    if (!str || str[0] == L'\0') {
        return FALSE;
    }

    for (int i = 0; str[i] != L'\0'; i++) {
        if (str[i] < L'0' || str[i] > L'9') {
            return FALSE;
        }
    }

    return TRUE;
}

// 将字符串转换为序号（用于GetProcAddress）
DWORD StringToOrdinal(LPCWSTR str)
{
    if (!str) {
        return 0;
    }

    DWORD ordinal = 0;
    for (int i = 0; str[i] != L'\0'; i++) {
        // 确保是数字字符
        if (str[i] < L'0' || str[i] > L'9') {
            return 0;
        }

        ordinal = ordinal * 10 + (str[i] - L'0');

        // 检查是否溢出（序号的低位WORD有效）
        if (ordinal > 0xFFFF) {
            return 0;
        }
    }

    return ordinal;
}

// 获取ANSI函数名（用于GetProcAddress）
LPCSTR GetAnsiFunctionName(LPCWSTR wideFunctionName)
{
    if (!wideFunctionName) {
        return NULL;
    }

    // 首先检查是否是数字字符串（导出序号）
    if (IsStringNumeric(wideFunctionName)) {
        DWORD ordinal = StringToOrdinal(wideFunctionName);
        if (ordinal > 0) {
            // 序号的低16位有效，高16位必须为0
            return (LPCSTR)(DWORD_PTR)ordinal;
        }
    }

    // 如果不是序号，则转换为ANSI字符串
    LPSTR ansiName = WideCharToAnsi(wideFunctionName);

    // 注意：调用者需要负责释放返回的字符串（如果是转换的）
    // 如果是序号，返回的是数值，不需要释放
    return ansiName;
}

// 释放转换后的字符串
void FreeConvertedString(LPVOID str)
{
    if (str) {
        free(str);
    }
}