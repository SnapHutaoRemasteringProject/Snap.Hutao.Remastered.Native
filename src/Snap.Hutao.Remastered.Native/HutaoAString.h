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
	
	void Clear();
	void Append(PCSTR value);
	void Append(const HutaoAString& value);
	
	int CompareTo(const HutaoAString& other) const;
	bool Equals(const HutaoAString& other) const;
	
	HutaoAString ToUpper() const;
	HutaoAString ToLower() const;
	HutaoAString Substring(size_t start, size_t length) const;
	HutaoAString Substring(size_t start) const;
	
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
	
	operator LPCSTR() const noexcept;
	
	static HutaoAString Format(PCSTR format, ...);
	static bool IsNullOrEmpty(const HutaoAString& str);
	static HutaoAString Join(const hutao::Array<HutaoAString>& values, PCSTR separator);

	static const HutaoAString Empty;

private:
	hutao::Array<char> m_buffer;
	
	void EnsureCapacity(size_t newCapacity);
	void UpdateNullTerminator();
	static size_t CalculateLength(PCSTR str);
	void InternalAppend(PCSTR value, size_t length);
};
