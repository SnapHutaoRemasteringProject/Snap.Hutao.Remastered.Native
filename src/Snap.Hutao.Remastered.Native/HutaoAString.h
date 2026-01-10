#pragma once

#include "IHutaoAString.h"
#include "CustomImplements.h"
#include "Array.h"
#include <Windows.h>
#include <cstdint>

#ifdef SNAPHUTAOREMASTEREDNATIVE_EXPORTS
#define HUTAOASTRING_API __declspec(dllexport)
#else
#define HUTAOASTRING_API __declspec(dllimport)
#endif

class HUTAOASTRING_API HutaoAString : public hutao::CustomImplements<HutaoAString, IHutaoAString>
{
public:
	HutaoAString();
	HutaoAString(PCSTR initialValue);
	HutaoAString(const char* initialValue, size_t length);
	HutaoAString(const HutaoAString& other);
	HutaoAString(HutaoAString&& other) noexcept;
	
	~HutaoAString();

	virtual HRESULT __stdcall GetBuffer(LPCSTR* buffer) noexcept override;

	size_t Length() const noexcept;
	bool IsEmpty() const noexcept;
	const char* Data() const noexcept;
	char* Data() noexcept;
	
	// 基本操作
	void Clear();
	void Append(PCSTR value);
	void Append(const HutaoAString& value);
	
	// 比较操作
	int CompareTo(const HutaoAString& other) const;
	bool Equals(const HutaoAString& other) const;
	
	// 转换操作
	HutaoAString ToUpper() const;
	HutaoAString ToLower() const;
	HutaoAString Substring(size_t start, size_t length) const;
	HutaoAString Substring(size_t start) const;
	
	// C#风格字符串方法
	bool Contains(PCSTR value) const;
	bool Contains(const HutaoAString& value) const;
	int IndexOf(char ch) const;
	int IndexOf(char ch, size_t startIndex) const;
	int IndexOf(PCSTR value) const;
	int LastIndexOf(char ch) const;
	int LastIndexOf(char ch, size_t startIndex) const;
	HutaoAString Replace(char oldChar, char newChar) const;
	HutaoAString Replace(PCSTR oldValue, PCSTR newValue) const;
	HutaoAString Trim() const;
	HutaoAString TrimStart() const;
	HutaoAString TrimEnd() const;
	hutao::Array<HutaoAString> Split(char separator) const;
	bool StartsWith(PCSTR value) const;
	bool EndsWith(PCSTR value) const;
	
	// 运算符重载
	HutaoAString& operator=(const HutaoAString& other);
	HutaoAString& operator=(HutaoAString&& other) noexcept;
	HutaoAString& operator=(PCSTR other);
	HutaoAString operator+(const HutaoAString& other) const;
	HutaoAString operator+(PCSTR other) const;
	HutaoAString& operator+=(const HutaoAString& other);
	HutaoAString& operator+=(PCSTR other);
	bool operator==(const HutaoAString& other) const;
	bool operator==(PCSTR other) const;
	bool operator!=(const HutaoAString& other) const;
	bool operator!=(PCSTR other) const;
	bool operator<(const HutaoAString& other) const;
	bool operator>(const HutaoAString& other) const;
	bool operator<=(const HutaoAString& other) const;
	bool operator>=(const HutaoAString& other) const;
	char operator[](size_t index) const;
	char& operator[](size_t index);
	
	// 转换运算符
	operator LPCSTR() const noexcept;
	
	// 静态方法
	static HutaoAString Format(PCSTR format, ...);
	static bool IsNullOrEmpty(const HutaoAString& str);
	static HutaoAString Join(const hutao::Array<HutaoAString>& values, PCSTR separator);

private:
	hutao::Array<char> m_buffer;  // 使用自定义Array，不使用std::string
	
	// 辅助方法
	void EnsureCapacity(size_t newCapacity);
	void UpdateNullTerminator();
	static size_t CalculateLength(PCSTR str);
	void InternalAppend(PCSTR value, size_t length);
};
