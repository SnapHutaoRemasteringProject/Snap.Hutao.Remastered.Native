#pragma once

#include "IHutaoString.h"
#include "CustomImplements.h"
#include "Array.h"
#include <Windows.h>
#include <cstdint>

#ifdef SNAPHUTAOREMASTEREDNATIVE_EXPORTS
#define HUTAOSTRING_API __declspec(dllexport)
#else
#define HUTAOSTRING_API __declspec(dllimport)
#endif

class HUTAOSTRING_API HutaoString : public hutao::CustomImplements<HutaoString, IHutaoString>
{
public:
	HutaoString();
	HutaoString(PCWSTR initialValue);
	HutaoString(const wchar_t* initialValue, size_t length);
	HutaoString(const HutaoString& other);
	HutaoString(HutaoString&& other) noexcept;
	
	~HutaoString();

	virtual HRESULT __stdcall GetBuffer(LPCWSTR* buffer) noexcept override;

	size_t Length() const noexcept;
	bool IsEmpty() const noexcept;
	const wchar_t* Data() const noexcept;
	wchar_t* Data() noexcept;
	
	void Clear();
	void Append(PCWSTR value);
	void Append(const wchar_t* value, size_t length);
	void Append(const HutaoString& value);
	void EnsureCapacity(size_t newCapacity);
	
	int CompareTo(const HutaoString& other) const;
	bool Equals(const HutaoString& other) const;
	
	HutaoString ToUpper() const;
	HutaoString ToLower() const;
	HutaoString Substring(size_t start, size_t length) const;
	HutaoString Substring(size_t start) const;
	
	bool Contains(PCWSTR value) const;
	bool Contains(const HutaoString& value) const;
	int IndexOf(wchar_t ch) const;
	int IndexOf(wchar_t ch, size_t startIndex) const;
	int IndexOf(PCWSTR value) const;
	int LastIndexOf(wchar_t ch) const;
	int LastIndexOf(wchar_t ch, size_t startIndex) const;
	HutaoString Replace(wchar_t oldChar, wchar_t newChar) const;
	HutaoString Replace(PCWSTR oldValue, PCWSTR newValue) const;
	HutaoString Trim() const;
	HutaoString TrimStart() const;
	HutaoString TrimEnd() const;
	hutao::Array<HutaoString> Split(wchar_t separator) const;
	bool StartsWith(PCWSTR value) const;
	bool EndsWith(PCWSTR value) const;
	
	HutaoString& operator=(const HutaoString& other);
	HutaoString& operator=(HutaoString&& other) noexcept;
	HutaoString& operator=(PCWSTR other);
	HutaoString operator+(const HutaoString& other) const;
	HutaoString operator+(PCWSTR other) const;
	HutaoString& operator+=(const HutaoString& other);
	HutaoString& operator+=(PCWSTR other);
	bool operator==(const HutaoString& other) const;
	bool operator==(PCWSTR other) const;
	bool operator!=(const HutaoString& other) const;
	bool operator!=(PCWSTR other) const;
	bool operator<(const HutaoString& other) const;
	bool operator>(const HutaoString& other) const;
	bool operator<=(const HutaoString& other) const;
	bool operator>=(const HutaoString& other) const;
	wchar_t operator[](size_t index) const;
	wchar_t& operator[](size_t index);
	
	operator LPCWSTR() const noexcept;
	
	static HutaoString Format(PCWSTR format, ...);
	static bool IsNullOrEmpty(const HutaoString& str);
	static HutaoString Join(const hutao::Array<HutaoString>& values, PCWSTR separator);

	static const HutaoString Empty;

private:
	hutao::Array<wchar_t> m_buffer;
	
	void UpdateNullTerminator();
	static size_t CalculateLength(PCWSTR str);
	void InternalAppend(PCWSTR value, size_t length);
};
