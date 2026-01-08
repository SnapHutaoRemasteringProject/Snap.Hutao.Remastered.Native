#include "HutaoNativeFileSystem.h"
#include "IHutaoString.h"
#include "HutaoString.h"
#include "CustomImplements.h"
#include "Error.h"
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <comdef.h>

HRESULT HutaoNativeFileSystem::PerformFileOperation(UINT operation, PCWSTR source, PCWSTR destination, long flags)
{
	AssertNonNullAndReturn(source);

    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = operation;
    
    size_t sourceLen = wcslen(source);
    std::vector<wchar_t> sourceBuffer(sourceLen + 2);
    wmemcpy(sourceBuffer.data(), source, sourceLen);
    sourceBuffer[sourceLen] = L'\0';
    sourceBuffer[sourceLen + 1] = L'\0';
    fileOp.pFrom = sourceBuffer.data();
    
    // Destination may be nullptr for delete operations
    if (destination != nullptr)
    {
        size_t destLen = wcslen(destination);
        std::vector<wchar_t> destBuffer(destLen + 2);
        wmemcpy(destBuffer.data(), destination, destLen);
        destBuffer[destLen] = L'\0';
        destBuffer[destLen + 1] = L'\0';
        fileOp.pTo = destBuffer.data();
    }
    
    fileOp.fFlags = static_cast<FILEOP_FLAGS>(flags);
    fileOp.hwnd = nullptr;
    fileOp.hNameMappings = nullptr;
    fileOp.lpszProgressTitle = nullptr;
    
    // Execute the operation
    int result = SHFileOperationW(&fileOp);
    
    if (result != 0)
    {
		ThrowForHR(HRESULT_FROM_WIN32(result), "File operation failed");
        return HRESULT_FROM_WIN32(result);
    }
    
    // Check if operation was cancelled
    if (fileOp.fAnyOperationsAborted)
    {
		ThrowForHR(HRESULT_FROM_WIN32(ERROR_CANCELLED), "File operation was cancelled");
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    
    return S_OK;
}

HRESULT HutaoNativeFileSystem::CreateHutaoStringFromWideString(PCWSTR wideString, IHutaoString** ppString)
{
	AssertNonNullAndReturn(ppString);
    
    if (wideString == nullptr)
    {
        *ppString = nullptr;
        return S_OK;
    }
    
    hutao::com_ptr<IHutaoString> stringObj = hutao::make_com_ptr<HutaoString>(wideString);
    *ppString = stringObj.detach();
    
    return S_OK;
}

HRESULT __stdcall HutaoNativeFileSystem::RenameItem(PCWSTR filePath, PCWSTR newName) noexcept
{
	AssertNonNullAndReturn(filePath);
	AssertNonNullAndReturn(newName);
    
    // For rename, we need to construct the new path
    std::wstring filePathStr(filePath);
    size_t lastSlash = filePathStr.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos)
    {
		ThrowForHR(E_INVALIDARG, "Invalid file path for rename operation");
        return E_INVALIDARG;
    }
    
    std::wstring newPath = filePathStr.substr(0, lastSlash + 1) + newName;
    
    return PerformFileOperation(FO_RENAME, filePath, newPath.c_str(), 0);
}

HRESULT __stdcall HutaoNativeFileSystem::RenameItemWithOptions(PCWSTR filePath, PCWSTR newName, long flags) noexcept
{
    AssertNonNullAndReturn(filePath);
    AssertNonNullAndReturn(newName);
    
    std::wstring filePathStr(filePath);
    size_t lastSlash = filePathStr.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos)
    {
		ThrowForHR(E_INVALIDARG, "Invalid file path for rename operation");
        return E_INVALIDARG;
    }
    
    std::wstring newPath = filePathStr.substr(0, lastSlash + 1) + newName;
    
    return PerformFileOperation(FO_RENAME, filePath, newPath.c_str(), flags);
}

HRESULT __stdcall HutaoNativeFileSystem::MoveItem(PCWSTR oldPath, PCWSTR newFolder) noexcept
{
	AssertNonNullAndReturn(oldPath);
	AssertNonNullAndReturn(newFolder);
    
    return PerformFileOperation(FO_MOVE, oldPath, newFolder, 0);
}

HRESULT __stdcall HutaoNativeFileSystem::MoveItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    
    return PerformFileOperation(FO_MOVE, oldPath, newFolder, flags);
}

HRESULT __stdcall HutaoNativeFileSystem::MoveItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
	AssertNonNullAndReturn(name);
    
    // Construct new path with new name
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_MOVE, oldPath, newPath.c_str(), 0);
}

HRESULT __stdcall HutaoNativeFileSystem::MoveItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    AssertNonNullAndReturn(name);
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_MOVE, oldPath, newPath.c_str(), flags);
}

HRESULT __stdcall HutaoNativeFileSystem::CopyItem(PCWSTR oldPath, PCWSTR newFolder) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    
    return PerformFileOperation(FO_COPY, oldPath, newFolder, 0);
}

HRESULT __stdcall HutaoNativeFileSystem::CopyItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    
    return PerformFileOperation(FO_COPY, oldPath, newFolder, flags);
}

HRESULT __stdcall HutaoNativeFileSystem::CopyItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    AssertNonNullAndReturn(name);
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_COPY, oldPath, newPath.c_str(), 0);
}

HRESULT __stdcall HutaoNativeFileSystem::CopyItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept
{
    AssertNonNullAndReturn(oldPath);
    AssertNonNullAndReturn(newFolder);
    AssertNonNullAndReturn(name);
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_COPY, oldPath, newPath.c_str(), flags);
}

HRESULT __stdcall HutaoNativeFileSystem::DeleteItem(PCWSTR filePath) noexcept
{
	AssertNonNullAndReturn(filePath);
    
    return PerformFileOperation(FO_DELETE, filePath, nullptr, 0);
}

HRESULT __stdcall HutaoNativeFileSystem::DeleteItemWithOptions(PCWSTR filePath, long flags) noexcept
{
	AssertNonNullAndReturn(filePath);
    
    return PerformFileOperation(FO_DELETE, filePath, nullptr, flags);
}

// IHutaoNativeFileSystem2 methods
HRESULT __stdcall HutaoNativeFileSystem::CreateLink(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName) noexcept
{
    AssertNonNullAndReturn(fileLocation);
    AssertNonNullAndReturn(fileName);
    
    // Create IShellLink object
    IShellLinkW* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pShellLink));
    hutao::com_ptr<IShellLinkW> shellLink(pShellLink);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "CoCreateInstance for IShellLink failed");
        return hr;
    }
    
    // Set path
    hr = shellLink->SetPath(fileLocation);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "IShellLink::SetPath failed");
        return hr;
    }
    
    // Set arguments if provided
    if (arguments != nullptr && wcslen(arguments) > 0)
    {
        hr = shellLink->SetArguments(arguments);
        if (FAILED(hr))
        {
            ThrowForHR(hr, "IShellLink::SetArguments failed");
            return hr;
        }
    }
    
    // Set icon location if provided
    if (iconLocation != nullptr && wcslen(iconLocation) > 0)
    {
        hr = shellLink->SetIconLocation(iconLocation, 0);
        if (FAILED(hr))
        {
            ThrowForHR(hr, "IShellLink::SetIconLocation failed");
            return hr;
        }
    }
    
    // Get IPersistFile interface
    IPersistFile* pPersistFile = nullptr;
    hr = shellLink->QueryInterface(IID_PPV_ARGS(&pPersistFile));
    if (FAILED(hr)) {
        ThrowForHR(hr, "QueryInterface for IPersistFile failed");
        return hr;
    }
    hutao::com_ptr<IPersistFile> persistFile(pPersistFile);
    
    // Save the link
    hr = persistFile->Save(fileName, TRUE);
    if (FAILED(hr))
    {
        ThrowForHR(hr, "IPersistFile::Save failed");
        return hr;
    }
    
    return S_OK;
}

// IHutaoNativeFileSystem3 methods
HRESULT __stdcall HutaoNativeFileSystem::PickFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept
{
    AssertNonNullAndReturn(picked);
    AssertNonNullAndReturn(path);
    
    *picked = FALSE;
    *path = nullptr;
    
    // Prepare OPENFILENAMEW structure
    OPENFILENAMEW ofn = { 0 };
    wchar_t fileBuffer[MAX_PATH] = { 0 };
    
    if (defaultFileName != nullptr)
    {
        wcsncpy_s(fileBuffer, defaultFileName, _TRUNCATE);
    }
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    
    if (title != nullptr)
    {
        ofn.lpstrTitle = title;
    }
    
    // Prepare filter string
    std::wstring filterStr;
    if (fileFilterName != nullptr && fileFilterType != nullptr)
    {
        filterStr = std::wstring(fileFilterName) + L'\0' + std::wstring(fileFilterType) + L'\0';
    }
    else
    {
        filterStr = L"All Files\0*.*\0";
    }
    
    ofn.lpstrFilter = filterStr.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    // Show open file dialog
    if (GetOpenFileNameW(&ofn))
    {
        *picked = TRUE;
        IHutaoString* pString = nullptr;
        HRESULT hr = CreateHutaoStringFromWideString(fileBuffer, &pString);
        if (SUCCEEDED(hr))
        {
            *path = pString;
        }
        else
        {
            ThrowForHR(hr, "CreateHutaoStringFromWideString failed in PickFile");
        }
        return hr;
    }
    
    // Check for cancellation
    DWORD error = CommDlgExtendedError();
    if (error == 0)
    {
        // User cancelled
        return S_OK;
    }
    
    HRESULT hrErr = HRESULT_FROM_WIN32(error);
    ThrowForHR(hrErr, "GetOpenFileNameW failed");
    return hrErr;
}

HRESULT __stdcall HutaoNativeFileSystem::SaveFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept
{
    AssertNonNullAndReturn(picked);
    AssertNonNullAndReturn(path);
    
    *picked = FALSE;
    *path = nullptr;
    
    // Prepare OPENFILENAMEW structure
    OPENFILENAMEW ofn = { 0 };
    wchar_t fileBuffer[MAX_PATH] = { 0 };
    
    if (defaultFileName != nullptr)
    {
        wcsncpy_s(fileBuffer, defaultFileName, _TRUNCATE);
    }
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fileBuffer;
    ofn.nMaxFile = MAX_PATH;
    
    if (title != nullptr)
    {
        ofn.lpstrTitle = title;
    }
    
    // Prepare filter string
    std::wstring filterStr;
    if (fileFilterName != nullptr && fileFilterType != nullptr)
    {
        filterStr = std::wstring(fileFilterName) + L'\0' + std::wstring(fileFilterType) + L'\0';
    }
    else
    {
        filterStr = L"All Files\0*.*\0";
    }
    
    ofn.lpstrFilter = filterStr.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    // Show save file dialog
    if (GetSaveFileNameW(&ofn))
    {
        *picked = TRUE;
        IHutaoString* pString = nullptr;
        HRESULT hr = CreateHutaoStringFromWideString(fileBuffer, &pString);
        if (SUCCEEDED(hr))
        {
            *path = pString;
        }
        else
        {
            ThrowForHR(hr, "CreateHutaoStringFromWideString failed in SaveFile");
        }
        return hr;
    }
    
    // Check for cancellation
    DWORD error = CommDlgExtendedError();
    if (error == 0)
    {
        // User cancelled
        return S_OK;
    }
    
    HRESULT hrErr = HRESULT_FROM_WIN32(error);
    ThrowForHR(hrErr, "GetSaveFileNameW failed");
    return hrErr;
}

HRESULT __stdcall HutaoNativeFileSystem::PickFolder(HWND hwnd, PCWSTR title, BOOL* picked, IHutaoString** path) noexcept
{
    AssertNonNullAndReturn(picked);
    AssertNonNullAndReturn(path);
    
    *picked = FALSE;
    *path = nullptr;
    
    // Initialize COM for BROWSEINFO
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        ThrowForHR(hr, "CoInitializeEx failed in PickFolder");
        return hr;
    }
    
    // Prepare BROWSEINFOW structure
    BROWSEINFOW bi = { 0 };
    bi.hwndOwner = hwnd;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    if (title != nullptr)
    {
        bi.lpszTitle = title;
    }
    
    // Show folder dialog
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (pidl != nullptr)
    {
        wchar_t folderPath[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, folderPath))
        {
            *picked = TRUE;
            IHutaoString* pString = nullptr;
            hr = CreateHutaoStringFromWideString(folderPath, &pString);
            if (SUCCEEDED(hr))
            {
                *path = pString;
            }
        }
        
        // Free PIDL
        CoTaskMemFree(pidl);
    }
    
    CoUninitialize();
    
    if (FAILED(hr))
    {
        ThrowForHR(hr, "PickFolder failed");
    }
    
    return hr;
}

// IHutaoNativeFileSystem4 methods
HRESULT __stdcall HutaoNativeFileSystem::CopyFileAllowDecryptedDestination(PCWSTR existingFileName, PCWSTR newFileName, BOOL overwrite) noexcept
{
    AssertNonNullAndReturn(existingFileName);
    AssertNonNullAndReturn(newFileName);
    
    // Use CopyFileExW with COPY_FILE_ALLOW_DECRYPTED_DESTINATION flag
    DWORD flags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION;
    if (!overwrite)
    {
        flags |= COPY_FILE_FAIL_IF_EXISTS;
    }
    
    BOOL result = CopyFileExW(existingFileName, newFileName, nullptr, nullptr, nullptr, flags);
    if (!result)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        ThrowForHR(hr, "CopyFileAllowDecryptedDestination failed");
        return hr;
    }
    
    return S_OK;
}
