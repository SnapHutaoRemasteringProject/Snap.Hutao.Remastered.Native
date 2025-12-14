#pragma once

#include <windows.h>
#include <cstdint>

// GUID 辅助宏 - 允许使用字符串格式的 GUID，类似 C# 的 Guid("...")
// 使用方法: DEFINE_GUID_STR(name, "F1F44E9E-858D-4746-B44E-213A1DDA4510")
// 这会自动将字符串转换为 DEFINE_GUID 需要的格式，避免手动转换字节时出错
// 
// GUID 字符串格式: "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
// 示例: "F1F44E9E-858D-4746-B44E-213A1DDA4510"

// 内部辅助宏：将单个十六进制字符转换为半字节（nibble）
#define _GUID_CHAR_TO_NIBBLE(c) \
    (((c) >= '0' && (c) <= '9') ? ((c) - '0') : \
     ((c) >= 'A' && (c) <= 'F') ? ((c) - 'A' + 10) : \
     ((c) >= 'a' && (c) <= 'f') ? ((c) - 'a' + 10) : 0)

// 内部辅助宏：将字符串中的两个十六进制字符转换为一个字节
// s: 字符串，i: 起始位置（跳过连字符）
#define _GUID_STR_TO_BYTE(s, i) \
    ((_GUID_CHAR_TO_NIBBLE((s)[(i)]) << 4) | _GUID_CHAR_TO_NIBBLE((s)[(i) + 1]))

// 主宏：将字符串格式的 GUID 转换为 DEFINE_GUID 格式
// 解析格式: "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
//          位置: 01234567-89AB-CDEF-GHIJ-KLMNOPQRSTUV
#define DEFINE_GUID_STR(name, guidStr) \
    DEFINE_GUID(name, \
        /* Data1: 前8个字符 (0-7) */ \
        ((uint32_t)(_GUID_STR_TO_BYTE(guidStr, 0) << 24) | \
         (uint32_t)(_GUID_STR_TO_BYTE(guidStr, 2) << 16) | \
         (uint32_t)(_GUID_STR_TO_BYTE(guidStr, 4) << 8) | \
         (uint32_t)(_GUID_STR_TO_BYTE(guidStr, 6))), \
        /* Data2: 第9-12个字符 (9-12, 跳过第8位的'-') */ \
        ((uint16_t)(_GUID_STR_TO_BYTE(guidStr, 9) << 8) | \
         (uint16_t)(_GUID_STR_TO_BYTE(guidStr, 11))), \
        /* Data3: 第14-17个字符 (14-17, 跳过第13位的'-') */ \
        ((uint16_t)(_GUID_STR_TO_BYTE(guidStr, 14) << 8) | \
         (uint16_t)(_GUID_STR_TO_BYTE(guidStr, 16))), \
        /* Data4[0-1]: 第19-22个字符 (19-22, 跳过第18位的'-') */ \
        _GUID_STR_TO_BYTE(guidStr, 19), \
        _GUID_STR_TO_BYTE(guidStr, 21), \
        /* Data4[2-7]: 第24-35个字符 (24-35, 跳过第23位的'-') */ \
        _GUID_STR_TO_BYTE(guidStr, 24), \
        _GUID_STR_TO_BYTE(guidStr, 26), \
        _GUID_STR_TO_BYTE(guidStr, 28), \
        _GUID_STR_TO_BYTE(guidStr, 30), \
        _GUID_STR_TO_BYTE(guidStr, 32), \
        _GUID_STR_TO_BYTE(guidStr, 34))
