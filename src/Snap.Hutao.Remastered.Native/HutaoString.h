#pragma once

#include "IHutaoString.h"
#include <winrt/base.h>
#include <Windows.h>
#include <cstdint>
#include <string>

class HutaoString : public winrt::implements<HutaoString, IHutaoString, winrt::non_agile>
{
public:
	HutaoString();
	HutaoString(PCWSTR initialValue);
	HutaoString(const std::wstring& initialValue);

	// IHutaoString 接口实现
	virtual HRESULT WINAPI GetBuffer(PCWSTR* buffer) noexcept;
	HRESULT WINAPI GetBufferSize(uint32_t* size) noexcept;
	HRESULT WINAPI SetBuffer(PCWSTR value, uint32_t length) noexcept;
	HRESULT WINAPI GetLength(uint32_t* length) noexcept;
	HRESULT WINAPI Clear() noexcept;
	HRESULT WINAPI Append(PCWSTR value) noexcept;
	HRESULT WINAPI CompareTo(IHutaoString* other, int32_t* result) noexcept;
	HRESULT WINAPI ToUpper() noexcept;
	HRESULT WINAPI ToLower() noexcept;
	HRESULT WINAPI Substring(uint32_t start, uint32_t length, IHutaoString** result) noexcept;

	std::wstring ToStdWString() const;
	LPWSTR ToCStr() const;
	uint32_t GetLength() const;
	bool IsEmpty() const;
	const wchar_t* Data() const;
	size_t Size() const;

	HutaoString& operator=(const std::wstring& other);
	HutaoString& operator=(PCWSTR other);
	bool operator==(const HutaoString& other) const;
	bool operator!=(const HutaoString& other) const;

	operator std::wstring() const;
	operator LPWSTR() const;

private:
	winrt::hstring m_buffer;

	void EnsureCapacity(size_t newCapacity);
	void UpdateInternalBuffer();
};

