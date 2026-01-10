#include "pch.h"
#include "HutaoString.h"
#include "Error.h"
#include <Windows.h>
#include <cwctype>
#include <cstdarg>
#include <cwchar>

// 构造函数
HutaoString::HutaoString() : m_buffer()
{
    m_buffer.Add(L'\0');
}

HutaoString::HutaoString(PCWSTR initialValue) : m_buffer()
{
    if (initialValue != nullptr) {
        size_t length = CalculateLength(initialValue);
        if (length > 0) {
            m_buffer.Reserve(length + 1);

            for (size_t i = 0; i < length; ++i) {
                m_buffer.Add(initialValue[i]);
            }
        }
        m_buffer.Add(L'\0');
    } else {
        m_buffer.Add(L'\0');
    }
}

HutaoString::HutaoString(const wchar_t* initialValue, size_t length) : m_buffer()
{
    if (initialValue != nullptr && length > 0) {
        m_buffer.Reserve(length + 1);

        for (size_t i = 0; i < length; ++i) {
            m_buffer.Add(initialValue[i]);
        }

        m_buffer.Add(L'\0');
    } else {
        m_buffer.Add(L'\0');
    }
}

HutaoString::HutaoString(const HutaoString& other) : m_buffer(other.m_buffer)
{
    // Array的拷贝构造函数已经处理了拷贝
}

HutaoString::HutaoString(HutaoString&& other) noexcept : m_buffer(std::move(other.m_buffer))
{
    // Array的移动构造函数已经处理了移动
}

// 析构函数
HutaoString::~HutaoString()
{
    // Array的析构函数会自动清理
}

// IHutaoString 接口实现
HRESULT __stdcall HutaoString::GetBuffer(LPCWSTR* buffer) noexcept
{
    AssertNonNullAndReturn(buffer);
    
    try {
        if (m_buffer.Length() == 0) {
            *buffer = L"";
        } else {
            *buffer = m_buffer.Data();
        }
        return S_OK;
    }
    catch (...) {
        return E_FAIL;
    }
}

// C#风格的方法（非接口方法）
size_t HutaoString::Length() const noexcept
{
    // 长度不包括null终止符
    return m_buffer.Length() > 0 ? m_buffer.Length() - 1 : 0;
}

bool HutaoString::IsEmpty() const noexcept
{
    return Length() == 0;
}

const wchar_t* HutaoString::Data() const noexcept
{
    return m_buffer.Length() > 0 ? m_buffer.Data() : L"";
}

wchar_t* HutaoString::Data() noexcept
{
    return m_buffer.Length() > 0 ? m_buffer.Data() : const_cast<wchar_t*>(L"");
}

// 基本操作
void HutaoString::Clear()
{
    m_buffer.Clear();
    // 添加null终止符
    m_buffer.Add(L'\0');
}

void HutaoString::Append(PCWSTR value)
{
    if (value == nullptr || value[0] == L'\0') {
        return;
    }
    
    size_t valueLength = CalculateLength(value);
    if (valueLength == 0) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    // 追加新内容
    for (size_t i = 0; i < valueLength; ++i) {
        m_buffer.Add(value[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add(L'\0');
}

void HutaoString::Append(const wchar_t* value, size_t length)
{
    if (value == nullptr || length == 0) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    // 追加新内容
    for (size_t i = 0; i < length; ++i) {
        m_buffer.Add(value[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add(L'\0');
}

void HutaoString::Append(const HutaoString& value)
{
    if (value.IsEmpty()) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    // 追加新内容
    const wchar_t* valueData = value.Data();
    size_t valueLength = value.Length();
    for (size_t i = 0; i < valueLength; ++i) {
        m_buffer.Add(valueData[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add(L'\0');
}

// 比较操作
int HutaoString::CompareTo(const HutaoString& other) const
{
    const wchar_t* thisData = this->Data();
    const wchar_t* otherData = other.Data();
    
    if (thisData == nullptr && otherData == nullptr) {
        return 0;
    }
    if (thisData == nullptr) {
        return -1;
    }
    if (otherData == nullptr) {
        return 1;
    }
    
    return wcscmp(thisData, otherData);
}

bool HutaoString::Equals(const HutaoString& other) const
{
    return CompareTo(other) == 0;
}

// 转换操作
HutaoString HutaoString::ToUpper() const
{
    HutaoString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        result.m_buffer.Add(towupper(thisData[i]));
    }
    result.m_buffer.Add(L'\0');
    
    return result;
}

HutaoString HutaoString::ToLower() const
{
    HutaoString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        result.m_buffer.Add(towlower(thisData[i]));
    }
    result.m_buffer.Add(L'\0');
    
    return result;
}

HutaoString HutaoString::Substring(size_t start, size_t length) const
{
    HutaoString result;
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
    
    const wchar_t* thisData = this->Data();

    if (result.m_buffer.Length() > 0) {
        result.m_buffer.RemoveAt(m_buffer.Length() - 1);
    }

    result.m_buffer.Reserve(actualLength + 1);
    
    for (size_t i = 0; i < actualLength; ++i) {
        result.m_buffer.Add(thisData[start + i]);
    }
    result.m_buffer.Add(L'\0');
    
    return result;
}

HutaoString HutaoString::Substring(size_t start) const {
    return Substring(start, Length() - start);
}

// C#风格字符串方法
bool HutaoString::Contains(PCWSTR value) const
{
    if (value == nullptr || value[0] == L'\0') {
        return true; 
    }
    
    const wchar_t* thisData = this->Data();
    if (thisData == nullptr || thisData[0] == L'\0') {
        return false;
    }
    
    return wcsstr(thisData, value) != nullptr;
}

bool HutaoString::Contains(const HutaoString& value) const
{
    return Contains(value.Data());
}

int HutaoString::IndexOf(wchar_t ch) const
{
    return IndexOf(ch, 0);
}

int HutaoString::IndexOf(wchar_t ch, size_t startIndex) const
{
    size_t len = Length();
    if (startIndex >= len) {
        return -1;
    }
    
    const wchar_t* thisData = this->Data();
    for (size_t i = startIndex; i < len; ++i) {
        if (thisData[i] == ch) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

int HutaoString::IndexOf(PCWSTR value) const
{
    if (value == nullptr || value[0] == L'\0') {
        return 0; // 空字符串在位置0
    }
    
    const wchar_t* thisData = this->Data();
    if (thisData == nullptr || thisData[0] == L'\0') {
        return -1; // 空字符串不包含任何非空字符串
    }
    
    const wchar_t* found = wcsstr(thisData, value);
    if (found == nullptr) {
        return -1;
    }
    
    return static_cast<int>(found - thisData);
}

int HutaoString::LastIndexOf(wchar_t ch) const
{
    size_t len = Length();
    if (len == 0) {
        return -1;
    }
    
    return LastIndexOf(ch, len - 1);
}

int HutaoString::LastIndexOf(wchar_t ch, size_t startIndex) const
{
    size_t len = Length();
    if (len == 0 || startIndex >= len) {
        return -1;
    }
    
    const wchar_t* thisData = this->Data();
    for (size_t i = startIndex + 1; i > 0; --i) {
        if (thisData[i - 1] == ch) {
            return static_cast<int>(i - 1);
        }
    }
    
    return -1;
}

HutaoString HutaoString::Replace(wchar_t oldChar, wchar_t newChar) const
{
    HutaoString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    result.m_buffer.Reserve(len + 1);
    
    for (size_t i = 0; i < len; ++i) {
        wchar_t c = thisData[i];
        result.m_buffer.Add(c == oldChar ? newChar : c);
    }
    result.m_buffer.Add(L'\0');
    
    return result;
}

HutaoString HutaoString::Replace(PCWSTR oldValue, PCWSTR newValue) const
{
    // 简化实现：使用wcstok和字符串拼接
    // 注意：这不是最高效的实现，但对于大多数情况足够
    HutaoString result;
    const wchar_t* thisData = this->Data();
    
    if (oldValue == nullptr || oldValue[0] == L'\0') {
        // 如果oldValue为空，返回原字符串
        result = *this;
        return result;
    }
    
    // 查找第一个匹配
    const wchar_t* found = wcsstr(thisData, oldValue);
    if (found == nullptr) {
        // 没有匹配，返回原字符串
        result = *this;
        return result;
    }
    
    // 计算新值长度
    size_t oldLen = CalculateLength(oldValue);
    size_t newLen = newValue ? CalculateLength(newValue) : 0;
    
    // 构建结果字符串
    size_t pos = found - thisData;
    result.Append(Substring(0, pos));
    if (newValue != nullptr) {
        result.Append(newValue);
    }
    result.Append(Substring(pos + oldLen, Length() - (pos + oldLen)));
    
    return result;
}

HutaoString HutaoString::Trim() const
{
    return TrimStart().TrimEnd();
}

HutaoString HutaoString::TrimStart() const
{
    HutaoString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    size_t start = 0;
    
    // 跳过开头的空白字符
    while (start < len && iswspace(thisData[start])) {
        ++start;
    }
    
    if (start >= len) {
        return result; // 全部是空白字符
    }
    
    return Substring(start, len - start);
}

HutaoString HutaoString::TrimEnd() const
{
    HutaoString result;
    size_t len = Length();
    if (len == 0) {
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    size_t end = len;
    
    // 跳过结尾的空白字符
    while (end > 0 && iswspace(thisData[end - 1])) {
        --end;
    }
    
    if (end == 0) {
        return result; // 全部是空白字符
    }
    
    return Substring(0, end);
}

hutao::Array<HutaoString> HutaoString::Split(wchar_t separator) const
{
    hutao::Array<HutaoString> result;
    size_t len = Length();
    if (len == 0) {
        result.Add(HutaoString()); // 添加一个空字符串
        return result;
    }
    
    const wchar_t* thisData = this->Data();
    size_t start = 0;
    
    for (size_t i = 0; i <= len; ++i) {
        if (i == len || thisData[i] == separator) {
            size_t partLength = i - start;
            if (partLength > 0 || (i > 0 && thisData[i - 1] == separator)) {
                result.Add(Substring(start, partLength));
            }
            start = i + 1;
        }
    }
    
    return result;
}

bool HutaoString::StartsWith(PCWSTR value) const
{
    if (value == nullptr || value[0] == L'\0') {
        return true; // 空字符串总是匹配开头
    }
    
    const wchar_t* thisData = this->Data();
    if (thisData == nullptr) {
        return false;
    }
    
    size_t valueLen = CalculateLength(value);
    if (valueLen > Length()) {
        return false;
    }
    
    return wcsncmp(thisData, value, valueLen) == 0;
}

bool HutaoString::EndsWith(PCWSTR value) const
{
    if (value == nullptr || value[0] == L'\0') {
        return true; // 空字符串总是匹配结尾
    }
    
    const wchar_t* thisData = this->Data();
    if (thisData == nullptr) {
        return false;
    }
    
    size_t thisLen = Length();
    size_t valueLen = CalculateLength(value);
    if (valueLen > thisLen) {
        return false;
    }
    
    return wcsncmp(thisData + thisLen - valueLen, value, valueLen) == 0;
}

// 运算符重载
HutaoString& HutaoString::operator=(const HutaoString& other)
{
    if (this != &other) {
        m_buffer = other.m_buffer;
    }
    return *this;
}

HutaoString& HutaoString::operator=(HutaoString&& other) noexcept
{
    if (this != &other) {
        m_buffer = std::move(other.m_buffer);
    }
    return *this;
}

HutaoString& HutaoString::operator=(PCWSTR other)
{
    Clear();
    if (other != nullptr) {
        Append(other);
    }
    return *this;
}

HutaoString HutaoString::operator+(const HutaoString& other) const
{
    HutaoString result = *this;
    result.Append(other);
    return result;
}

HutaoString HutaoString::operator+(PCWSTR other) const
{
    HutaoString result = *this;
    result.Append(other);
    return result;
}

HutaoString& HutaoString::operator+=(const HutaoString& other)
{
    Append(other);
    return *this;
}

HutaoString& HutaoString::operator+=(PCWSTR other)
{
    Append(other);
    return *this;
}

bool HutaoString::operator==(const HutaoString& other) const
{
    return Equals(other);
}

bool HutaoString::operator==(PCWSTR other) const
{
    if (other == nullptr) {
        return IsEmpty();
    }
    
    const wchar_t* thisData = this->Data();
    if (thisData == nullptr) {
        return other[0] == L'\0';
    }
    
    return wcscmp(thisData, other) == 0;
}

bool HutaoString::operator!=(const HutaoString& other) const
{
    return !Equals(other);
}

bool HutaoString::operator!=(PCWSTR other) const
{
    return !(*this == other);
}

bool HutaoString::operator<(const HutaoString& other) const
{
    return CompareTo(other) < 0;
}

bool HutaoString::operator>(const HutaoString& other) const
{
    return CompareTo(other) > 0;
}

bool HutaoString::operator<=(const HutaoString& other) const
{
    return CompareTo(other) <= 0;
}

bool HutaoString::operator>=(const HutaoString& other) const
{
    return CompareTo(other) >= 0;
}

wchar_t HutaoString::operator[](size_t index) const
{
    size_t len = Length();
    if (index >= len) {
        return L'\0';
    }
    return this->Data()[index];
}

wchar_t& HutaoString::operator[](size_t index)
{
    static wchar_t dummy = L'\0';
    size_t len = Length();
    if (index >= len) {
        return dummy;
    }
    return m_buffer[index];
}

// 转换运算符
HutaoString::operator LPCWSTR() const noexcept
{
    return Data();
}

// 静态方法
HutaoString HutaoString::Format(PCWSTR format, ...)
{
    if (format == nullptr) {
        return HutaoString();
    }
    
    va_list args;
    va_start(args, format);
    
    // 获取需要的缓冲区大小
    int needed = _vscwprintf(format, args);
    if (needed < 0) {
        va_end(args);
        return HutaoString();
    }
    
    // 分配缓冲区
    wchar_t* buffer = new wchar_t[needed + 1];
    vswprintf_s(buffer, needed + 1, format, args);
    va_end(args);
    
    HutaoString result(buffer);
    delete[] buffer;
    
    return result;
}

bool HutaoString::IsNullOrEmpty(const HutaoString& str)
{
    return str.IsEmpty();
}

HutaoString HutaoString::Join(const hutao::Array<HutaoString>& values, PCWSTR separator)
{
    HutaoString result;
    
    if (values.Length() == 0) {
        return result; // 返回空字符串
    }
    
    // 计算总长度
    size_t totalLength = 0;
    size_t separatorLength = separator ? CalculateLength(separator) : 0;
    
    for (size_t i = 0; i < values.Length(); ++i) {
        totalLength += values[i].Length();
        if (i < values.Length() - 1 && separatorLength > 0) {
            totalLength += separatorLength;
        }
    }
    
    // 预留容量
    result.m_buffer.Reserve(totalLength + 1);
    
    // 构建结果字符串
    for (size_t i = 0; i < values.Length(); ++i) {
        const HutaoString& value = values[i];
        result.Append(value.Data());
        
        if (i < values.Length() - 1 && separatorLength > 0) {
            result.Append(separator);
        }
    }
    
    return result;
}

// 私有辅助方法实现
size_t HutaoString::CalculateLength(PCWSTR str)
{
    if (str == nullptr) {
        return 0;
    }
    
    size_t length = 0;
    while (str[length] != L'\0') {
        ++length;
    }
    
    return length;
}

void HutaoString::EnsureCapacity(size_t newCapacity)
{
    if (newCapacity > m_buffer.Capacity()) {
        // 使用Array的Reserve方法
        m_buffer.Reserve(newCapacity);
    }
}

void HutaoString::UpdateNullTerminator()
{
    // 确保缓冲区以null终止符结束
    if (m_buffer.Length() == 0 || m_buffer[m_buffer.Length() - 1] != L'\0') {
        m_buffer.Add(L'\0');
    }
}

void HutaoString::InternalAppend(PCWSTR value, size_t length)
{
    if (value == nullptr || length == 0) {
        return;
    }
    
    // 移除当前的null终止符
    if (m_buffer.Length() > 0) {
        m_buffer.RemoveAt(m_buffer.Length() - 1);
    }
    
    // 追加新内容
    for (size_t i = 0; i < length; ++i) {
        m_buffer.Add(value[i]);
    }
    
    // 添加新的null终止符
    m_buffer.Add(L'\0');
}
