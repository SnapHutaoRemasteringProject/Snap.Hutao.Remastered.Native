#include "pch.h"
#include "HutaoNative.h"
#include "HutaoNativeRegistryNotification.h"
#include "IHutaoNativeLoopbackSupport.h"
#include "IHutaoNativeRegistryNotification.h"
#include "IHutaoNativeWindowNonRude.h"
#include "IHutaoNativeWindowSubclass.h"
#include "HutaoNativeLoopbackSupport.h"
#include "HutaoNativeWindowSubclass.h"
#include "HutaoNativeWindowSubclassCallback.h"
#include "HutaoNativeDeviceCapabilities.h"
#include "HutaoNativePhysicalDrive.h"
#include "HutaoNativeLogicalDrive.h"
#include "HutaoNativeInputLowLevelKeyboardSource.h"
#include "HutaoNativeFileSystem.h"
#include "HutaoNativeNotifyIcon.h"
#include "HutaoNativeHotKeyAction.h"
#include "HutaoNativeProcess.h"
#include "HutaoNativeWindowNonRude.h"

HRESULT __stdcall HutaoNative::MakeLoopbackSupport(IHutaoNativeLoopbackSupport** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeLoopbackSupport> support = hutao::make_com_ptr<HutaoNativeLoopbackSupport>();
	*ppv = support.detach();

	return S_OK;
}

HRESULT __stdcall HutaoNative::MakeRegistryNotification(PCWSTR keyPath, IHutaoNativeRegistryNotification** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeRegistryNotification> notify = hutao::make_com_ptr<HutaoNativeRegistryNotification>(keyPath);
	*ppv = notify.detach();

	return S_OK;
}

HRESULT __stdcall HutaoNative::MakeWindowSubclass(HWND hWnd, HutaoNativeWindowSubclassCallback callback, GCHandle userData, IHutaoNativeWindowSubclass** ppv)
{
	AssertNonNullAndReturn(ppv);

	// Convert GCHandle to LONG_PTR for constructor
	LONG_PTR userDataPtr = static_cast<LONG_PTR>(userData);

	// Pass the callback structure directly, not the function pointer
	hutao::com_ptr<IHutaoNativeWindowSubclass> subclass = hutao::make_com_ptr<HutaoNativeWindowSubclass>(hWnd, callback, userDataPtr);
	*ppv = subclass.detach();

	return S_OK;
}

HRESULT __stdcall HutaoNative::MakeWindowNonRude(HWND hWnd, IHutaoNativeWindowNonRude** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeWindowNonRude> nonRude = hutao::make_com_ptr<HutaoNativeWindowNonRude>(hWnd);
	*ppv = nonRude.detach();

	return S_OK;
}

// IHutaoNative2 methods
HRESULT __stdcall HutaoNative::MakeDeviceCapabilities(IHutaoNativeDeviceCapabilities** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeDeviceCapabilities> capabilities = hutao::make_com_ptr<HutaoNativeDeviceCapabilities>();
	*ppv = capabilities.detach();

	return S_OK;
}

HRESULT __stdcall HutaoNative::MakePhysicalDrive(IHutaoNativePhysicalDrive** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativePhysicalDrive> physicalDrive = hutao::make_com_ptr<HutaoNativePhysicalDrive>();
	*ppv = physicalDrive.detach();

	return S_OK;
}

HRESULT __stdcall HutaoNative::MakeLogicalDrive(IHutaoNativeLogicalDrive** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeLogicalDrive> logicalDrive = hutao::make_com_ptr<HutaoNativeLogicalDrive>();
	*ppv = logicalDrive.detach();

	return S_OK;
}

// IHutaoNative3 methods
HRESULT __stdcall HutaoNative::MakeInputLowLevelKeyboardSource(IHutaoNativeInputLowLevelKeyboardSource** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeInputLowLevelKeyboardSource> source = hutao::make_com_ptr<HutaoNativeInputLowLevelKeyboardSource>();
	*ppv = source.detach();

	return S_OK;
}

// IHutaoNative4 methods
HRESULT __stdcall HutaoNative::MakeFileSystem(IHutaoNativeFileSystem** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeFileSystem> fileSystem = hutao::make_com_ptr<HutaoNativeFileSystem>();
	*ppv = fileSystem.detach();

	return S_OK;
}

// IHutaoNative5 methods
HRESULT __stdcall HutaoNative::MakeNotifyIcon(PCWSTR iconPath, GUID* id, IHutaoNativeNotifyIcon** ppv)
{
	AssertNonNullAndReturn(ppv);

	// Pass iconPath to constructor
	hutao::com_ptr<IHutaoNativeNotifyIcon> notifyIcon = hutao::make_com_ptr<HutaoNativeNotifyIcon>(iconPath);
	*ppv = notifyIcon.detach();

	// Mark unused parameter to avoid warning
	(void)id;

	return S_OK;
}

// IHutaoNative6 methods
HRESULT __stdcall HutaoNative::MakeHotKeyAction(HutaoNativeHotKeyActionKind kind, HutaoNativeHotKeyActionCallback callback, GCHandle userData, IHutaoNativeHotKeyAction** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeHotKeyAction> hotKeyAction = hutao::make_com_ptr<HutaoNativeHotKeyAction>(kind, callback, userData);
	*ppv = hotKeyAction.detach();

	return S_OK;
}

// IHutaoNative7 methods
HRESULT __stdcall HutaoNative::MakeProcess(HutaoNativeProcessStartInfo info, IHutaoNativeProcess** ppv)
{
	AssertNonNullAndReturn(ppv);

	hutao::com_ptr<IHutaoNativeProcess> process = hutao::make_com_ptr<HutaoNativeProcess>(info);
	*ppv = process.detach();

	// Mark unused parameter to avoid warning
	(void)info;

	return S_OK;
}

static bool IsVersionGreaterOrEqual(
	const HutaoPrivateWindowsVersion& v,
	DWORD major,
	DWORD minor,
	DWORD build,
	DWORD revision)
{
	if (v.major != major)
		return v.major > major;

	if (v.minor != minor)
		return v.minor > minor;

	if (v.build != build)
		return v.build > build;

	return v.revision >= revision;
}

// IHutaoNativePrivate methods
HRESULT __stdcall HutaoNative::IsCurrentWindowsVersionSupported(BOOL* isSupported)
{
	AssertNonNullAndReturn(isSupported);

	HutaoPrivateWindowsVersion winver{};
	HRESULT hr = GetWindowsVersion(&winver);
	if (FAILED(hr))
	{
		return hr;
	}

	const DWORD targetMajor = 10;
	const DWORD targetMinor = 0;
	const DWORD targetBuild = 19045;
	const DWORD targetRevision = 5371;

	*isSupported = IsVersionGreaterOrEqual(
		winver,
		targetMajor,
		targetMinor,
		targetBuild,
		targetRevision)
		? TRUE
		: FALSE;

	return S_OK;
}

HRESULT __stdcall HutaoNative::GetWindowsVersion(HutaoPrivateWindowsVersion* pv)
{
	AssertNonNullAndReturn(pv);

	using RtlGetVersionPtr = NTSTATUS(NTAPI*)(PRTL_OSVERSIONINFOW);

	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	auto pRtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(hNtdll, "RtlGetVersion"));
	if (!pRtlGetVersion)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	RTL_OSVERSIONINFOW info{};
	info.dwOSVersionInfoSize = sizeof(info);

	NTSTATUS status = pRtlGetVersion(&info);
	if (!NT_SUCCESS(status))
	{
		return HRESULT_FROM_NT(status);
	}

	pv->major = info.dwMajorVersion;
	pv->minor = info.dwMinorVersion;
	pv->build = info.dwBuildNumber;

	// 获取 revision (UBR)
	DWORD ubr = 0;
	DWORD size = sizeof(ubr);

	if (RegGetValueW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
		L"UBR",
		RRF_RT_REG_DWORD,
		nullptr,
		&ubr,
		&size) == ERROR_SUCCESS)
	{
		pv->revision = ubr;
	}
	else
	{
		pv->revision = 0;
	}

	return S_OK;
}

HRESULT __stdcall HutaoNative::ShowErrorMessage(PCWSTR title, PCWSTR message)
{
	AssertNonNullAndReturn(title);
	AssertNonNullAndReturn(message);

	// Show message box
	int result = MessageBoxW(
		nullptr,           // No parent window
		message,           // Message text
		title,             // Title
		MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST
	);

	// MessageBoxW returns IDOK (1) when user clicks OK button
	// We always return S_OK because message box was shown successfully
	return S_OK;
}

// IHutaoPrivate2 methods
HRESULT __stdcall HutaoNative::ExchangeGameUidForIdentifier1820(PCWSTR gameUid, byte* identifier)
{
	AssertNonNullAndReturn(gameUid);
	AssertNonNullAndReturn(identifier);

	// Calculate string length (excluding null terminator)
	size_t length = 0;
	while (gameUid[length] != L'\0')
	{
		length++;
	}

	// Copy string to output buffer as UTF-16 byte sequence
	// Each character is 2 bytes (wchar_t)
	size_t byteCount = length * sizeof(wchar_t);
	memcpy(identifier, gameUid, byteCount);

	return S_OK;
}
