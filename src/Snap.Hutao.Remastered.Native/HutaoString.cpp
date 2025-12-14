#pragma once

#include "HutaoString.h"
#include <algorithm>
#include <winrt/base.h>
#include <Windows.h>
#include <cstdint>
#include <string>
#include "IHutaoString_h.h"
#include <cwctype>

namespace winrt
{
	using namespace Windows::Foundation;
}

HutaoString::HutaoString() : m_capacity(0)
{
	m_buffer.clear();
}

HutaoString::HutaoString(PCWSTR initialValue) : m_capacity(0)
{
	if (initialValue != nullptr)
	{
		m_buffer = initialValue;
	}
}

HutaoString::HutaoString(const std::wstring& initialValue)
	: m_buffer(initialValue), m_capacity(0)
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
		*size = static_cast<uint32_t>(m_buffer.capacity());
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
			m_buffer.clear();
		}
		else
		{
			if (length == static_cast<uint32_t>(-1))
			{
				m_buffer = value;
			}
			else
			{
				m_buffer.assign(value, length);
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
		*length = static_cast<uint32_t>(m_buffer.length());
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
		m_buffer.clear();
		m_buffer.shrink_to_fit();
		m_capacity = 0;
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
		m_buffer += value;
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

		int comparison = m_buffer.compare(otherBuffer);
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
		std::transform(m_buffer.begin(), m_buffer.end(), m_buffer.begin(),
			[](wchar_t c) { return std::towupper(c); });
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
		std::transform(m_buffer.begin(), m_buffer.end(), m_buffer.begin(),
			[](wchar_t c) { return std::towlower(c); });
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
		if (start >= m_buffer.length())
		{
			return E_BOUNDS;
		}

		size_t actualLength = length;
		if (length == static_cast<uint32_t>(-1) || start + length > m_buffer.length())
		{
			actualLength = m_buffer.length() - start;
		}

		std::wstring substring = m_buffer.substr(start, actualLength);
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
	return m_buffer;
}

LPWSTR HutaoString::ToCStr() const
{
	return ((LPWSTR)m_buffer.c_str());
}

uint32_t HutaoString::GetLength() const
{
	return m_buffer.length();
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
	m_buffer = other;
	return *this;
}

HutaoString& HutaoString::operator=(PCWSTR other)
{
	if (other != nullptr)
	{
		m_buffer = other;
	}
	else
	{
		m_buffer.clear();
	}
	return *this;
}

bool HutaoString::operator==(const HutaoString& other) const
{
	return m_buffer == other.m_buffer;
}

bool HutaoString::operator!=(const HutaoString& other) const
{
	return m_buffer != other.m_buffer;
}

HutaoString::operator std::wstring() const
{
	return m_buffer;
}

HutaoString::operator LPWSTR() const
{
	return ((LPWSTR)m_buffer.c_str());
}

void HutaoString::EnsureCapacity(size_t newCapacity)
{
	if (newCapacity > m_buffer.capacity())
	{
		m_buffer.reserve(newCapacity);
	}
}

void HutaoString::UpdateInternalBuffer()
{
	// 如果需要，可以在这里添加额外的缓冲管理逻辑
	if (m_buffer.capacity() > m_capacity)
	{
		m_capacity = m_buffer.capacity();
	}
}