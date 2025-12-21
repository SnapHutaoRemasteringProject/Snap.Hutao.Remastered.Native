#pragma once

#include "IHutaoString.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <cstdint>
#include <string>

class HutaoString : public hutao::CustomImplements<HutaoString, IHutaoString>
{
public:
	HutaoString();
	HutaoString(PCWSTR initialValue);
	HutaoString(const std::wstring& initialValue);

	// IHutaoString 接口实现
	virtual HRESULT __stdcall GetBuffer(PCWSTR* buffer) noexcept;
	HRESULT __stdcall GetBufferSize(uint32_t* size) noexcept;
	HRESULT __stdcall SetBuffer(PCWSTR value, uint32_t length) noexcept;
	HRESULT __stdcall GetLength(uint32_t* length) noexcept;
	HRESULT __stdcall Clear() noexcept;
	HRESULT __stdcall Append(PCWSTR value) noexcept;
	HRESULT __stdcall CompareTo(IHutaoString* other, int32_t* result) noexcept;
	HRESULT __stdcall ToUpper() noexcept;
	HRESULT __stdcall ToLower() noexcept;
	HRESULT __stdcall Substring(uint32_t start, uint32_t length, IHutaoString** result) noexcept;

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
	std::wstring m_buffer;

	void EnsureCapacity(size_t newCapacity);
	void UpdateInternalBuffer();
};
