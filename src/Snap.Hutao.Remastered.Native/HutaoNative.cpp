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
#include <Lmcons.h>
#include <comdef.h>
#include <taskschd.h>
#include <strsafe.h>
#include <string>

#pragma comment(lib, "Taskschd.lib")
#pragma comment(lib, "Ole32.lib")

namespace
{
	constexpr DWORD USERNAME_DOMAIN_LEN = DNLEN + UNLEN + 2;
	constexpr DWORD USERNAME_LEN = UNLEN + 1;

	HRESULT GetCurrentUserAndTaskName(_Out_writes_z_(USERNAME_DOMAIN_LEN) WCHAR* usernameDomain, _Out_writes_z_(USERNAME_LEN) WCHAR* username, std::wstring& taskName)
	{
		if (!GetEnvironmentVariableW(L"USERNAME", username, USERNAME_LEN))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (!GetEnvironmentVariableW(L"USERDOMAIN", usernameDomain, USERNAME_DOMAIN_LEN))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		HRESULT hr = StringCchCatW(usernameDomain, USERNAME_DOMAIN_LEN, L"\\");
		if (FAILED(hr))
		{
			return hr;
		}

		hr = StringCchCatW(usernameDomain, USERNAME_DOMAIN_LEN, username);
		if (FAILED(hr))
		{
			return hr;
		}

		taskName = L"Autorun for ";
		taskName += username;
		return S_OK;
	}

	HRESULT ConnectTaskService(ITaskService** ppService)
	{
		AssertNonNullAndReturn(ppService);
		*ppService = nullptr;

		HRESULT hr = CoCreateInstance(
			CLSID_TaskScheduler,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			reinterpret_cast<void**>(ppService));
		if (FAILED(hr))
		{
			return hr;
		}

		hr = (*ppService)->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
		if (FAILED(hr))
		{
			(*ppService)->Release();
			*ppService = nullptr;
		}

		return hr;
	}

	HRESULT GetOrCreateHutaoFolder(ITaskService* pService, ITaskFolder** ppTaskFolder)
	{
		AssertNonNullAndReturn(pService);
		AssertNonNullAndReturn(ppTaskFolder);
		*ppTaskFolder = nullptr;

		HRESULT hr = pService->GetFolder(_bstr_t(L"\\Hutao"), ppTaskFolder);
		if (SUCCEEDED(hr))
		{
			return S_OK;
		}

		ITaskFolder* pRootFolder = nullptr;
		hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = pRootFolder->CreateFolder(_bstr_t(L"\\Hutao"), _variant_t(L""), ppTaskFolder);
		pRootFolder->Release();
		return hr;
	}

	HRESULT GetHutaoFolder(ITaskService* pService, ITaskFolder** ppTaskFolder)
	{
		AssertNonNullAndReturn(pService);
		AssertNonNullAndReturn(ppTaskFolder);
		*ppTaskFolder = nullptr;
		return pService->GetFolder(_bstr_t(L"\\Hutao"), ppTaskFolder);
	}

	HRESULT InternalCreateAutoStartTaskForThisUser(bool runElevated)
	{
		HRESULT hr = S_OK;
		WCHAR usernameDomain[USERNAME_DOMAIN_LEN] = {};
		WCHAR username[USERNAME_LEN] = {};
		std::wstring taskName;

		hr = GetCurrentUserAndTaskName(usernameDomain, username, taskName);
		if (FAILED(hr))
		{
			return hr;
		}

		WCHAR executablePath[MAX_PATH] = {};
		if (!GetModuleFileNameW(nullptr, executablePath, MAX_PATH))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		ITaskService* pService = nullptr;
		ITaskFolder* pTaskFolder = nullptr;
		ITaskDefinition* pTask = nullptr;
		IRegistrationInfo* pRegInfo = nullptr;
		ITaskSettings* pSettings = nullptr;
		ITriggerCollection* pTriggerCollection = nullptr;
		IRegisteredTask* pRegisteredTask = nullptr;

		hr = ConnectTaskService(&pService);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = GetOrCreateHutaoFolder(pService, &pTaskFolder);
		if (FAILED(hr))
		{
			goto LExit;
		}

		{
			IRegisteredTask* pExistingRegisteredTask = nullptr;
			hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pExistingRegisteredTask);
			if (SUCCEEDED(hr) && pExistingRegisteredTask)
			{
				hr = pExistingRegisteredTask->put_Enabled(VARIANT_TRUE);
				pExistingRegisteredTask->Release();
				goto LExit;
			}
		}

		hr = pService->NewTask(0, &pTask);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pTask->get_RegistrationInfo(&pRegInfo);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pRegInfo->put_Author(_bstr_t(usernameDomain));
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pTask->get_Settings(&pSettings);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pSettings->put_StartWhenAvailable(VARIANT_FALSE);
		if (FAILED(hr))
		{
			goto LExit;
		}
		hr = pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
		if (FAILED(hr))
		{
			goto LExit;
		}
		hr = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S"));
		if (FAILED(hr))
		{
			goto LExit;
		}
		hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
		if (FAILED(hr))
		{
			goto LExit;
		}
		hr = pSettings->put_Priority(4);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pTask->get_Triggers(&pTriggerCollection);
		if (FAILED(hr))
		{
			goto LExit;
		}

		{
			ITrigger* pTrigger = nullptr;
			ILogonTrigger* pLogonTrigger = nullptr;

			hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pTrigger->QueryInterface(IID_ILogonTrigger, reinterpret_cast<void**>(&pLogonTrigger));
			pTrigger->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pLogonTrigger->put_Id(_bstr_t(L"Trigger1"));
			if (SUCCEEDED(hr))
			{
				hr = pLogonTrigger->put_Delay(_bstr_t(L"PT03S"));
			}
			if (SUCCEEDED(hr))
			{
				hr = pLogonTrigger->put_UserId(_bstr_t(usernameDomain));
			}

			pLogonTrigger->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}
		}

		{
			IActionCollection* pActionCollection = nullptr;
			IAction* pAction = nullptr;
			IExecAction* pExecAction = nullptr;

			hr = pTask->get_Actions(&pActionCollection);
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
			pActionCollection->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pAction->QueryInterface(IID_IExecAction, reinterpret_cast<void**>(&pExecAction));
			pAction->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pExecAction->put_Path(_bstr_t(executablePath));
			pExecAction->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}
		}

		{
			IPrincipal* pPrincipal = nullptr;
			hr = pTask->get_Principal(&pPrincipal);
			if (FAILED(hr))
			{
				goto LExit;
			}

			hr = pPrincipal->put_Id(_bstr_t(L"Principal1"));
			if (SUCCEEDED(hr))
			{
				hr = pPrincipal->put_UserId(_bstr_t(usernameDomain));
			}
			if (SUCCEEDED(hr))
			{
				hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
			}
			if (SUCCEEDED(hr))
			{
				hr = pPrincipal->put_RunLevel(runElevated ? TASK_RUNLEVEL_HIGHEST : TASK_RUNLEVEL_LUA);
			}

			pPrincipal->Release();
			if (FAILED(hr))
			{
				goto LExit;
			}
		}

		{
			_variant_t fullAccessForEveryone = L"D:(A;;FA;;;WD)";
			hr = pTaskFolder->RegisterTaskDefinition(
				_bstr_t(taskName.c_str()),
				pTask,
				TASK_CREATE_OR_UPDATE,
				_variant_t(usernameDomain),
				_variant_t(),
				TASK_LOGON_INTERACTIVE_TOKEN,
				fullAccessForEveryone,
				&pRegisteredTask);
		}

	LExit:
		if (pService)
		{
			pService->Release();
		}
		if (pTaskFolder)
		{
			pTaskFolder->Release();
		}
		if (pTask)
		{
			pTask->Release();
		}
		if (pRegInfo)
		{
			pRegInfo->Release();
		}
		if (pSettings)
		{
			pSettings->Release();
		}
		if (pTriggerCollection)
		{
			pTriggerCollection->Release();
		}
		if (pRegisteredTask)
		{
			pRegisteredTask->Release();
		}

		return hr;
	}

	HRESULT InternalDeleteAutoStartTaskForThisUser()
	{
		HRESULT hr = S_OK;
		WCHAR usernameDomain[USERNAME_DOMAIN_LEN] = {};
		WCHAR username[USERNAME_LEN] = {};
		std::wstring taskName;

		hr = GetCurrentUserAndTaskName(usernameDomain, username, taskName);
		if (FAILED(hr))
		{
			return hr;
		}

		ITaskService* pService = nullptr;
		ITaskFolder* pTaskFolder = nullptr;

		hr = ConnectTaskService(&pService);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = GetHutaoFolder(pService, &pTaskFolder);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				hr = S_OK;
			}
			goto LExit;
		}

		hr = pTaskFolder->DeleteTask(_bstr_t(taskName.c_str()), 0);
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
		{
			hr = S_OK;
		}

	LExit:
		if (pService)
		{
			pService->Release();
		}
		if (pTaskFolder)
		{
			pTaskFolder->Release();
		}

		return hr;
	}

	HRESULT InternalIsAutoStartTaskActiveForThisUser(BOOL* isActive)
	{
		AssertNonNullAndReturn(isActive);
		*isActive = FALSE;

		HRESULT hr = S_OK;
		WCHAR usernameDomain[USERNAME_DOMAIN_LEN] = {};
		WCHAR username[USERNAME_LEN] = {};
		std::wstring taskName;
		VARIANT_BOOL isEnabled = VARIANT_FALSE;

		hr = GetCurrentUserAndTaskName(usernameDomain, username, taskName);
		if (FAILED(hr))
		{
			return hr;
		}

		ITaskService* pService = nullptr;
		ITaskFolder* pTaskFolder = nullptr;
		IRegisteredTask* pExistingRegisteredTask = nullptr;

		hr = ConnectTaskService(&pService);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = GetHutaoFolder(pService, &pTaskFolder);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				hr = S_OK;
			}
			goto LExit;
		}

		hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pExistingRegisteredTask);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				hr = S_OK;
			}
			goto LExit;
		}

		hr = pExistingRegisteredTask->get_Enabled(&isEnabled);
		if (SUCCEEDED(hr))
		{
			*isActive = isEnabled == VARIANT_TRUE ? TRUE : FALSE;
		}

	LExit:
		if (pService)
		{
			pService->Release();
		}
		if (pTaskFolder)
		{
			pTaskFolder->Release();
		}
		if (pExistingRegisteredTask)
		{
			pExistingRegisteredTask->Release();
		}

		return hr;
	}

	HRESULT InternalIsAutoStartTaskRunElevatedForThisUser(BOOL* isRunElevated)
	{
		AssertNonNullAndReturn(isRunElevated);
		*isRunElevated = FALSE;

		HRESULT hr = S_OK;
		WCHAR usernameDomain[USERNAME_DOMAIN_LEN] = {};
		WCHAR username[USERNAME_LEN] = {};
		std::wstring taskName;
		TASK_RUNLEVEL_TYPE runLevel = TASK_RUNLEVEL_LUA;

		hr = GetCurrentUserAndTaskName(usernameDomain, username, taskName);
		if (FAILED(hr))
		{
			return hr;
		}

		ITaskService* pService = nullptr;
		ITaskFolder* pTaskFolder = nullptr;
		IRegisteredTask* pExistingRegisteredTask = nullptr;
		ITaskDefinition* pDef = nullptr;
		IPrincipal* pPrincipal = nullptr;

		hr = ConnectTaskService(&pService);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = GetHutaoFolder(pService, &pTaskFolder);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				hr = S_OK;
			}
			goto LExit;
		}

		hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pExistingRegisteredTask);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || hr == HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND))
			{
				hr = S_OK;
			}
			goto LExit;
		}

		hr = pExistingRegisteredTask->get_Definition(&pDef);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pDef->get_Principal(&pPrincipal);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pPrincipal->get_RunLevel(&runLevel);
		if (SUCCEEDED(hr))
		{
			*isRunElevated = runLevel == TASK_RUNLEVEL_HIGHEST ? TRUE : FALSE;
		}

	LExit:
		if (pService)
		{
			pService->Release();
		}
		if (pTaskFolder)
		{
			pTaskFolder->Release();
		}
		if (pExistingRegisteredTask)
		{
			pExistingRegisteredTask->Release();
		}
		if (pDef)
		{
			pDef->Release();
		}
		if (pPrincipal)
		{
			pPrincipal->Release();
		}

		return hr;
	}

	HRESULT InternalGetAutoStartTaskExecutablePathForThisUser(_Out_writes_z_(cchBuffer) WCHAR* buffer, DWORD cchBuffer)
	{
		AssertNonNullAndReturn(buffer);
		if (cchBuffer == 0)
		{
			return E_INVALIDARG;
		}

		buffer[0] = L'\0';

		HRESULT hr = S_OK;
		WCHAR usernameDomain[USERNAME_DOMAIN_LEN] = {};
		WCHAR username[USERNAME_LEN] = {};
		std::wstring taskName;

		hr = GetCurrentUserAndTaskName(usernameDomain, username, taskName);
		if (FAILED(hr))
		{
			return hr;
		}

		ITaskService* pService = nullptr;
		ITaskFolder* pTaskFolder = nullptr;
		IRegisteredTask* pExistingRegisteredTask = nullptr;
		ITaskDefinition* pDef = nullptr;
		IActionCollection* pActions = nullptr;
		IAction* pAction = nullptr;
		IExecAction* pExecAction = nullptr;
		BSTR bstrPath = nullptr;

		hr = ConnectTaskService(&pService);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = GetHutaoFolder(pService, &pTaskFolder);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pTaskFolder->GetTask(_bstr_t(taskName.c_str()), &pExistingRegisteredTask);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pExistingRegisteredTask->get_Definition(&pDef);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pDef->get_Actions(&pActions);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pActions->get_Item(1, &pAction);
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pAction->QueryInterface(IID_IExecAction, reinterpret_cast<void**>(&pExecAction));
		if (FAILED(hr))
		{
			goto LExit;
		}

		hr = pExecAction->get_Path(&bstrPath);
		if (FAILED(hr))
		{
			goto LExit;
		}

		if (!bstrPath)
		{
			hr = E_NOT_FOUND;
			goto LExit;
		}

		hr = StringCchCopyW(buffer, cchBuffer, bstrPath);

	LExit:
		if (bstrPath)
		{
			SysFreeString(bstrPath);
		}
		if (pService)
		{
			pService->Release();
		}
		if (pTaskFolder)
		{
			pTaskFolder->Release();
		}
		if (pExistingRegisteredTask)
		{
			pExistingRegisteredTask->Release();
		}
		if (pDef)
		{
			pDef->Release();
		}
		if (pActions)
		{
			pActions->Release();
		}
		if (pAction)
		{
			pAction->Release();
		}
		if (pExecAction)
		{
			pExecAction->Release();
		}

		return hr;
	}
}

HRESULT EnsureTaskSchedulerComInitialized(bool* shouldUninitialize)
{
	AssertNonNullAndReturn(shouldUninitialize);
	*shouldUninitialize = false;

	HRESULT hrInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (hrInit == RPC_E_CHANGED_MODE)
	{
		return S_OK;
	}

	if (FAILED(hrInit))
	{
		return hrInit;
	}

	*shouldUninitialize = true;
	return S_OK;
}

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

// IHutaoPrivate3 methods
HRESULT __stdcall HutaoNative::IsAutoStartTaskActiveForThisUser(BOOL* isActive)
{
	AssertNonNullAndReturn(isActive);

	bool shouldUninitialize = false;
	HRESULT hr = EnsureTaskSchedulerComInitialized(&shouldUninitialize);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = InternalIsAutoStartTaskActiveForThisUser(isActive);
	if (shouldUninitialize)
	{
		CoUninitialize();
	}

	return hr;
}

HRESULT __stdcall HutaoNative::CreateAutoStartTaskForThisUser(BOOL runElevated)
{
	bool shouldUninitialize = false;
	HRESULT hr = EnsureTaskSchedulerComInitialized(&shouldUninitialize);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = InternalCreateAutoStartTaskForThisUser(runElevated == TRUE);
	if (shouldUninitialize)
	{
		CoUninitialize();
	}

	return hr;
}

HRESULT __stdcall HutaoNative::DeleteAutoStartTaskForThisUser()
{
	bool shouldUninitialize = false;
	HRESULT hr = EnsureTaskSchedulerComInitialized(&shouldUninitialize);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = InternalDeleteAutoStartTaskForThisUser();
	if (shouldUninitialize)
	{
		CoUninitialize();
	}

	return hr;
}

HRESULT __stdcall HutaoNative::IsAutoStartTaskRunElevatedForThisUser(BOOL* isRunElevated)
{
	AssertNonNullAndReturn(isRunElevated);

	bool shouldUninitialize = false;
	HRESULT hr = EnsureTaskSchedulerComInitialized(&shouldUninitialize);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = InternalIsAutoStartTaskRunElevatedForThisUser(isRunElevated);
	if (shouldUninitialize)
	{
		CoUninitialize();
	}

	return hr;
}

HRESULT __stdcall HutaoNative::GetAutoStartTaskExecutablePathForThisUser(WCHAR* buffer, DWORD cchBuffer)
{
	AssertNonNullAndReturn(buffer);
	if (cchBuffer == 0)
	{
		return E_INVALIDARG;
	}

	bool shouldUninitialize = false;
	HRESULT hr = EnsureTaskSchedulerComInitialized(&shouldUninitialize);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = InternalGetAutoStartTaskExecutablePathForThisUser(buffer, cchBuffer);
	if (shouldUninitialize)
	{
		CoUninitialize();
	}

	return hr;
}
