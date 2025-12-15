#pragma once

#include "HutaoString.h"
#include "IHutaoString_h.h"
#include <algorithm>
#include <winrt/base.h>
#include <Windows.h>
#include <cstdint>
#include <string>
#include <cwctype>

namespace winrt
{
	using namespace Windows::Foundation;
}

HutaoString::HutaoString() : m_buffer(L"")
{
}

HutaoString::HutaoString(PCWSTR initialValue) : m_buffer(winrt::hstring(initialValue))
{
}

HutaoString::HutaoString(const std::wstring& initialValue)
	: m_buffer(winrt::hstring(initialValue))
{
}

HRESULT WINAPI HutaoString::GetBuffer(PCWSTR* buffer) noexcept
{
	if (buffer == nullptr)
	{
		return E_POINTER;
	}

	try
	{
		*buffer = m_buffer.c_str();
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::GetBufferSize(uint32_t* size) noexcept
{
	if (size == nullptr)
	{
		return E_POINTER;
	}

	try
	{
		*size = static_cast<uint32_t>(m_buffer.size());
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::SetBuffer(PCWSTR value, uint32_t length) noexcept
{
	if (value == nullptr && length > 0)
	{
		return E_INVALIDARG;
	}

	try
	{
		if (value == nullptr)
		{
			m_buffer = winrt::hstring();
		}
		else
		{
			if (length == static_cast<uint32_t>(-1))
			{
				m_buffer = winrt::hstring(value);
			}
			else
			{
				m_buffer = winrt::hstring(value, length);
			}
		}

		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::GetLength(uint32_t* length) noexcept
{
	if (length == nullptr)
	{
		return E_POINTER;
	}

	try
	{
		*length = static_cast<uint32_t>(m_buffer.size());
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::Clear() noexcept
{
	try
	{
		m_buffer = winrt::hstring();
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::Append(PCWSTR value) noexcept
{
	if (value == nullptr)
	{
		return S_OK;
	}

	try
	{
		m_buffer = m_buffer + winrt::hstring(value);
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::CompareTo(IHutaoString* other, int32_t* result) noexcept
{
	if (other == nullptr || result == nullptr)
	{
		return E_POINTER;
	}

	try
	{
		PCWSTR otherBuffer = nullptr;
		HRESULT hr = other->GetBuffer(&otherBuffer);
		if (FAILED(hr))
		{
			return hr;
		}

		if (otherBuffer == nullptr)
		{
			*result = m_buffer.empty() ? 0 : 1;
			return S_OK;
		}

		int comparison = std::wcscmp(m_buffer.c_str(), otherBuffer);
		if (comparison < 0)
		{
			*result = -1;
		}
		else if (comparison > 0)
		{
			*result = 1;
		}
		else
		{
			*result = 0;
		}

		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::ToUpper() noexcept
{
	try
	{
		std::wstring temp(m_buffer.c_str());
		std::transform(temp.begin(), temp.end(), temp.begin(),
			[](wchar_t c) { return std::towupper(c); });
		m_buffer = winrt::hstring(temp);
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::ToLower() noexcept
{
	try
	{
		std::wstring temp(m_buffer.c_str());
		std::transform(temp.begin(), temp.end(), temp.begin(),
			[](wchar_t c) { return std::towlower(c); });
		m_buffer = winrt::hstring(temp);
		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

HRESULT WINAPI HutaoString::Substring(uint32_t start, uint32_t length, IHutaoString** result) noexcept
{
	if (result == nullptr)
	{
		return E_POINTER;
	}

	*result = nullptr;

	try
	{
		std::wstring source(m_buffer.c_str());
		if (start >= source.length())
		{
			return E_BOUNDS;
		}

		size_t actualLength = length;
		if (length == static_cast<uint32_t>(-1) || start + length > source.length())
		{
			actualLength = source.length() - start;
		}

		std::wstring substring = source.substr(start, actualLength);
		winrt::com_ptr<IHutaoString> hutaoSubstring = winrt::make_self<HutaoString>(substring);
		*result = winrt::detach_abi(hutaoSubstring);

		return S_OK;
	}
	catch (...)
	{
		return winrt::to_hresult();
	}
}

std::wstring HutaoString::ToStdWString() const
{
	return std::wstring(m_buffer.c_str());
}

LPWSTR HutaoString::ToCStr() const
{
	return const_cast<LPWSTR>(m_buffer.c_str());
}

uint32_t HutaoString::GetLength() const
{
	return static_cast<uint32_t>(m_buffer.size());
}

bool HutaoString::IsEmpty() const
{
	return m_buffer.empty();
}

const wchar_t* HutaoString::Data() const
{
	return m_buffer.c_str();
}

size_t HutaoString::Size() const
{
	return m_buffer.size();
}

HutaoString& HutaoString::operator=(const std::wstring& other)
{
	m_buffer = winrt::hstring(other);
	return *this;
}

HutaoString& HutaoString::operator=(PCWSTR other)
{
	if (other != nullptr)
	{
		m_buffer = winrt::hstring(other);
	}
	else
	{
		m_buffer = winrt::hstring();
	}
	return *this;
}

bool HutaoString::operator==(const HutaoString& other) const
{
	return std::wcscmp(m_buffer.c_str(), other.m_buffer.c_str()) == 0;
}

bool HutaoString::operator!=(const HutaoString& other) const
{
	return !(*this == other);
}

HutaoString::operator std::wstring() const
{
	return std::wstring(m_buffer.c_str());
}

HutaoString::operator LPWSTR() const
{
	return const_cast<LPWSTR>(m_buffer.c_str());
}

void HutaoString::EnsureCapacity(size_t newCapacity)
{
	// hstring manages its own capacity; no-op
	(void)newCapacity;
}

void HutaoString::UpdateInternalBuffer()
{
	// hstring manages its own capacity; no-op
}