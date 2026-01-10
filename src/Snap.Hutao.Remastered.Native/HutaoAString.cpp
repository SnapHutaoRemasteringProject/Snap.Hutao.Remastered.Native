#include "pch.h"
#include "HutaoAString.h"
#include "Error.h"
#include <Windows.h>
#include <cctype>
#include <cstdarg>
#include <cstring>

HutaoAString::HutaoAString() : m_buffer()
{
    m_buffer.Add('\0');
}

HutaoAString::HutaoAString(PCSTR initialValue) : m_buffer()
{
    if (initialValue != nullptr) {
        size_t length = CalculateLength(initialValue);
        if (length > 0) {
            m_buffer.Reserve(length + 1);
            for (size_t i = 0; i < length; ++i) {
                m_buffer.Add(initialValue[i]);
            }
        }

        m_buffer.Add('\0');
    } else {
        m_buffer.Add('\0');
    }
}

HutaoAString::HutaoAString(const char* initialValue, size_t length) : m_buffer()
{
    if (initialValue != nullptr && length > 0) {
        m_buffer.Reserve(length + 1);
        for (size_t i = 0; i < length; ++i) {
            m_buffer.Add(initialValue[i]);
        }

        m_buffer.Add('\0');
    } else {
        m_buffer.Add('\0');
    }
}

HutaoAString::HutaoAString(const HutaoAString& other) : m_buffer(other.m_buffer)
{
    // Array的拷贝构造函数已经处理了拷贝
}

HutaoAString::HutaoAString(HutaoAString&& other) noexcept : m_buffer(std::move(other.m_buffer))
{
    // Array的移动构造函数已经处理了移动
}

HutaoAString::~HutaoAString()
{
    // Array的析构函数会自动清理
}

HRESULT __stdcall HutaoAString::GetBuffer(LPCSTR* buffer) noexcept
{
    AssertNonNullAndReturn(buffer);
    
    try {
        if (m_buffer.Length() == 0) {
            *buffer = "";
        } else {
            *buffer = m_buffer.Data();
        }
        return S_OK;
    }
    catch (...) {
        return E_FAIL;
    }
}

size_t HutaoAString::Length() const noexcept
{
    return m_buffer.Length() > 0 ? m_buffer.Length() - 1 : 0;
}

bool HutaoAString::IsEmpty() const noexcept
{
    return Length() == 0;
}

const char* HutaoAString::Data() const noexcept
{
    return m_buffer.Length() > 0 ? m_buffer.Data() : "";
}

char* HutaoAString::Data() noexcept
{
    return m_buffer.Length() > 0 ? m_buffer.Data() : const_cast<char*>("");
}

// 基本操作
void HutaoAString::Clear()
{
    m_buffer.Clear();
    m_buffer.Add('\0');
}

void HutaoAString::Append(PCSTR value)
{
    if (value == nullptr || value[0] == '\0') {
        return;
    }
    
    size_t valueLength = CalculateLength(value);
    if (valueLength == 0) {
        return;
    }
    
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    for (size_t i = 0; i < valueLength; ++i) {
        m_buffer.Add(value[i]);
    }
    
    m_buffer.Add('\0');
}

void HutaoAString::Append(const HutaoAString& value)
{
    if (value.IsEmpty()) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    const char* valueData = value.Data();
    size_t valueLength = value.Length();
    for (size_t i = 0; i < valueLength; ++i) {
        m_buffer.Add(valueData[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add('\0');
}

// 比较操作
int HutaoAString::CompareTo(const HutaoAString& other) const
{
    const char* thisData = Data();
    const char* otherData = other.Data();
    
    if (thisData == nullptr && otherData == nullptr) {
        return 0;
    }
    if (thisData == nullptr) {
        return -1;
    }
    if (otherData == nullptr) {
        return 1;
    }
    
    return strcmp(thisData, otherData);
}

bool HutaoAString::Equals(const HutaoAString& other) const
{
    return CompareTo(other) == 0;
}

// 转换操作
HutaoAString HutaoAString::ToUpper() const
{
    HutaoAString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const char* data = Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        result.m_buffer.Add(toupper(static_cast<unsigned char>(data[i])));
    }
    result.m_buffer.Add('\0');
    
    return result;
}

HutaoAString HutaoAString::ToLower() const
{
    HutaoAString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const char* data = Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        result.m_buffer.Add(tolower(static_cast<unsigned char>(data[i])));
    }
    result.m_buffer.Add('\0');
    
    return result;
}

HutaoAString HutaoAString::Substring(size_t start, size_t length) const
{
    HutaoAString result;
    size_t thisLength = Length();
    
    if (start >= thisLength) {
        return result;
    }
    
    size_t actualLength = length;
    if (start + length > thisLength) {
        actualLength = thisLength - start;
    }
    
    if (actualLength == 0) {
        return result;
    }
    
    const char* data = Data();

    if (result.m_buffer.Length() > 0) {
        result.m_buffer.RemoveAt(m_buffer.Length() - 1);
    }

    result.m_buffer.Reserve(actualLength + 1);
    
    for (size_t i = 0; i < actualLength; ++i) {
        result.m_buffer.Add(data[start + i]);
    }
    result.m_buffer.Add('\0');
    
    return result;
}

HutaoAString HutaoAString::Substring(size_t start) const {
    return Substring(start, Length() - start);
}

bool HutaoAString::Contains(PCSTR value) const
{
    if (value == nullptr || value[0] == '\0') {
        return true;
    }
    
    const char* thisData = Data();
    if (thisData == nullptr || thisData[0] == '\0') {
        return false;
    }
    
    return strstr(thisData, value) != nullptr;
}

bool HutaoAString::Contains(const HutaoAString& value) const
{
    return Contains(value.Data());
}

int HutaoAString::IndexOf(char ch) const
{
    return IndexOf(ch, 0);
}

int HutaoAString::IndexOf(char ch, size_t startIndex) const
{
    size_t len = Length();
    if (startIndex >= len) {
        return -1;
    }
    
    const char* data = Data();
    for (size_t i = startIndex; i < len; ++i) {
        if (data[i] == ch) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

int HutaoAString::IndexOf(PCSTR value) const
{
    if (value == nullptr || value[0] == '\0') {
        return 0;
    }
    
    const char* thisData = Data();
    if (thisData == nullptr || thisData[0] == '\0') {
        return -1;
    }
    
    const char* found = strstr(thisData, value);
    if (found == nullptr) {
        return -1;
    }
    
    return static_cast<int>(found - thisData);
}

int HutaoAString::LastIndexOf(char ch) const
{
    size_t len = Length();
    if (len == 0) {
        return -1;
    }
    
    return LastIndexOf(ch, len - 1);
}

int HutaoAString::LastIndexOf(char ch, size_t startIndex) const
{
    size_t len = Length();
    if (len == 0 || startIndex >= len) {
        return -1;
    }
    
    const char* data = Data();
    for (size_t i = startIndex + 1; i > 0; --i) {
        if (data[i - 1] == ch) {
            return static_cast<int>(i - 1);
        }
    }
    
    return -1;
}

HutaoAString HutaoAString::Replace(char oldChar, char newChar) const
{
    HutaoAString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const char* data = Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        char c = data[i];
        result.m_buffer.Add(c == oldChar ? newChar : c);
    }
    result.m_buffer.Add('\0');
    
    return result;
}

HutaoAString HutaoAString::Replace(PCSTR oldValue, PCSTR newValue) const
{
    HutaoAString result;
    const char* thisData = Data();
    
    if (oldValue == nullptr || oldValue[0] == '\0') {
        result = *this;
        return result;
    }
    
    const char* found = strstr(thisData, oldValue);
    if (found == nullptr) {
        result = *this;
        return result;
    }
    
    size_t oldLen = CalculateLength(oldValue);
    size_t newLen = newValue ? CalculateLength(newValue) : 0;
    
    size_t pos = found - thisData;
    result.Append(Substring(0, pos));
    if (newValue != nullptr) {
        result.Append(newValue);
    }
    result.Append(Substring(pos + oldLen, Length() - (pos + oldLen)));
    
    return result;
}

HutaoAString HutaoAString::Trim() const
{
    return TrimStart().TrimEnd();
}

HutaoAString HutaoAString::TrimStart() const
{
    HutaoAString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const char* data = Data();
    size_t start = 0;
    
    while (start < len && isspace(static_cast<unsigned char>(data[start]))) {
        ++start;
    }
    
    if (start >= len) {
        return result;
    }
    
    return Substring(start, len - start);
}

HutaoAString HutaoAString::TrimEnd() const
{
    HutaoAString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const char* data = Data();
    size_t end = len;
    
    while (end > 0 && isspace(static_cast<unsigned char>(data[end - 1]))) {
        --end;
    }
    
    if (end == 0) {
        return result; // 全部是空白字符
    }
    
    return Substring(0, end);
}

hutao::Array<HutaoAString> HutaoAString::Split(char separator) const
{
    hutao::Array<HutaoAString> result;
    size_t len = Length();
    if (len == 0) {
        result.Add(HutaoAString());
        return result;
    }
    
    const char* data = Data();
    size_t start = 0;
    
    for (size_t i = 0; i <= len; ++i) {
        if (i == len || data[i] == separator) {
            size_t partLength = i - start;
            if (partLength > 0 || (i > 0 && data[i - 1] == separator)) {
                result.Add(Substring(start, partLength));
            }
            start = i + 1;
        }
    }
    
    return result;
}

bool HutaoAString::StartsWith(PCSTR value) const
{
    if (value == nullptr || value[0] == '\0') {
        return true; // 空字符串总是匹配开头
    }
    
    const char* thisData = Data();
    if (thisData == nullptr) {
        return false;
    }
    
    size_t valueLen = CalculateLength(value);
    if (valueLen > Length()) {
        return false;
    }
    
    return strncmp(thisData, value, valueLen) == 0;
}

bool HutaoAString::EndsWith(PCSTR value) const
{
    if (value == nullptr || value[0] == '\0') {
        return true; // 空字符串总是匹配结尾
    }
    
    const char* thisData = Data();
    if (thisData == nullptr) {
        return false;
    }
    
    size_t thisLen = Length();
    size_t valueLen = CalculateLength(value);
    if (valueLen > thisLen) {
        return false;
    }
    
    return strncmp(thisData + thisLen - valueLen, value, valueLen) == 0;
}

// 运算符重载
HutaoAString& HutaoAString::operator=(const HutaoAString& other)
{
    if (this != &other) {
        m_buffer = other.m_buffer;
    }
    return *this;
}

HutaoAString& HutaoAString::operator=(HutaoAString&& other) noexcept
{
    if (this != &other) {
        m_buffer = std::move(other.m_buffer);
    }
    return *this;
}

HutaoAString& HutaoAString::operator=(PCSTR other)
{
    Clear();
    if (other != nullptr) {
        Append(other);
    }
    return *this;
}

HutaoAString HutaoAString::operator+(const HutaoAString& other) const
{
    HutaoAString result = *this;
    result.Append(other);
    return result;
}

HutaoAString HutaoAString::operator+(PCSTR other) const
{
    HutaoAString result = *this;
    result.Append(other);
    return result;
}

HutaoAString& HutaoAString::operator+=(const HutaoAString& other)
{
    Append(other);
    return *this;
}

HutaoAString& HutaoAString::operator+=(PCSTR other)
{
    Append(other);
    return *this;
}

bool HutaoAString::operator==(const HutaoAString& other) const
{
    return Equals(other);
}

bool HutaoAString::operator==(PCSTR other) const
{
    if (other == nullptr) {
        return IsEmpty();
    }
    
    const char* thisData = Data();
    if (thisData == nullptr) {
        return other[0] == '\0';
    }
    
    return strcmp(thisData, other) == 0;
}

bool HutaoAString::operator!=(const HutaoAString& other) const
{
    return !Equals(other);
}

bool HutaoAString::operator!=(PCSTR other) const
{
    return !(*this == other);
}

bool HutaoAString::operator<(const HutaoAString& other) const
{
    return CompareTo(other) < 0;
}

bool HutaoAString::operator>(const HutaoAString& other) const
{
    return CompareTo(other) > 0;
}

bool HutaoAString::operator<=(const HutaoAString& other) const
{
    return CompareTo(other) <= 0;
}

bool HutaoAString::operator>=(const HutaoAString& other) const
{
    return CompareTo(other) >= 0;
}

char HutaoAString::operator[](size_t index) const
{
    size_t len = Length();
    if (index >= len) {
        return '\0';
    }
    return Data()[index];
}

char& HutaoAString::operator[](size_t index)
{
    static char dummy = '\0';
    size_t len = Length();
    if (index >= len) {
        return dummy;
    }
    return m_buffer[index];
}

// 转换运算符
HutaoAString::operator LPCSTR() const noexcept
{
    return Data();
}

// 静态方法
HutaoAString HutaoAString::Format(PCSTR format, ...)
{
    if (format == nullptr) {
        return HutaoAString();
    }
    
    va_list args;
    va_start(args, format);
    
    // 获取需要的缓冲区大小
    int neededSize = _vscprintf(format, args);
    if (neededSize < 0) {
        va_end(args);
        return HutaoAString();
    }
    
    // 分配缓冲区（包括null终止符）
    size_t bufferSize = static_cast<size_t>(neededSize) + 1;
    char* buffer = new char[bufferSize];
    
    // 格式化字符串
    int result = vsprintf_s(buffer, bufferSize, format, args);
    va_end(args);
    
    if (result < 0) {
        delete[] buffer;
        return HutaoAString();
    }
    
    // 创建HutaoAString对象
    HutaoAString str(buffer);
    delete[] buffer;
    
    return str;
}

bool HutaoAString::IsNullOrEmpty(const HutaoAString& str)
{
    return str.IsEmpty();
}

HutaoAString HutaoAString::Join(const hutao::Array<HutaoAString>& values, PCSTR separator)
{
    HutaoAString result;
    if (values.Length() == 0) {
        return result;
    }
    
    size_t separatorLength = separator ? CalculateLength(separator) : 0;
    
    // 计算总长度
    size_t totalLength = 0;
    for (size_t i = 0; i < values.Length(); ++i) {
        totalLength += values[i].Length();
    }
    
    // 添加分隔符长度
    if (separatorLength > 0 && values.Length() > 1) {
        totalLength += separatorLength * (values.Length() - 1);
    }
    
    // 预留容量
    result.m_buffer.Reserve(totalLength + 1);
    
    // 构建字符串
    for (size_t i = 0; i < values.Length(); ++i) {
        if (i > 0 && separatorLength > 0) {
            result.InternalAppend(separator, separatorLength);
        }
        
        const HutaoAString& value = values[i];
        if (!value.IsEmpty()) {
            result.InternalAppend(value.Data(), value.Length());
        }
    }
    
    // 确保有null终止符
    result.UpdateNullTerminator();
    
    return result;
}

// 私有辅助方法
void HutaoAString::EnsureCapacity(size_t newCapacity)
{
    if (newCapacity > m_buffer.Capacity()) {
        m_buffer.Reserve(newCapacity);
    }
}

void HutaoAString::UpdateNullTerminator()
{
    // 确保最后一个字符是null终止符
    if (m_buffer.Length() == 0) {
        m_buffer.Add('\0');
    } else {
        // 如果最后一个字符不是null终止符，添加一个
        if (m_buffer[m_buffer.Length() - 1] != '\0') {
            m_buffer.Add('\0');
        }
    }
}

size_t HutaoAString::CalculateLength(PCSTR str)
{
    if (str == nullptr) {
        return 0;
    }
    
    size_t length = 0;
    while (str[length] != '\0') {
        ++length;
    }
    
    return length;
}

void HutaoAString::InternalAppend(PCSTR value, size_t length)
{
    if (value == nullptr || length == 0) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0 && m_buffer[m_buffer.Length() - 1] == '\0') {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    // 追加新内容
    for (size_t i = 0; i < length; ++i) {
        m_buffer.Add(value[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add('\0');
}
