#include "HutaoNativeFileSystem.h"
#include "IHutaoString.h"
#include "HutaoString.h"
#include <winrt/base.h>
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <comdef.h>

using namespace winrt;

// Helper function to perform file operations using SHFileOperationW
HRESULT HutaoNativeFileSystem::PerformFileOperation(UINT operation, PCWSTR source, PCWSTR destination, long flags)
{
    if (source == nullptr)
    {
        return E_INVALIDARG;
    }

    // Prepare SHFILEOPSTRUCTW
    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = operation;
    
    // Source must be double-null terminated
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
        return HRESULT_FROM_WIN32(result);
    }
    
    // Check if operation was cancelled
    if (fileOp.fAnyOperationsAborted)
    {
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    
    return S_OK;
}

// Helper function to create IHutaoString from wide string
HRESULT HutaoNativeFileSystem::CreateHutaoStringFromWideString(PCWSTR wideString, IHutaoString** ppString)
{
    if (ppString == nullptr)
    {
        return E_POINTER;
    }
    
    if (wideString == nullptr)
    {
        *ppString = nullptr;
        return S_OK;
    }
    
    com_ptr<IHutaoString> stringObj = make_self<HutaoString>(wideString);
    *ppString = detach_abi(stringObj);
    
    return S_OK;
}

// IHutaoNativeFileSystem methods
HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::RenameItem(PCWSTR filePath, PCWSTR newName) noexcept
{
    if (filePath == nullptr || newName == nullptr)
    {
        return E_INVALIDARG;
    }
    
    // For rename, we need to construct the new path
    std::wstring filePathStr(filePath);
    size_t lastSlash = filePathStr.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos)
    {
        return E_INVALIDARG;
    }
    
    std::wstring newPath = filePathStr.substr(0, lastSlash + 1) + newName;
    
    return PerformFileOperation(FO_RENAME, filePath, newPath.c_str(), 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::RenameItemWithOptions(PCWSTR filePath, PCWSTR newName, long flags) noexcept
{
    if (filePath == nullptr || newName == nullptr)
    {
        return E_INVALIDARG;
    }
    
    std::wstring filePathStr(filePath);
    size_t lastSlash = filePathStr.find_last_of(L"\\/");
    if (lastSlash == std::wstring::npos)
    {
        return E_INVALIDARG;
    }
    
    std::wstring newPath = filePathStr.substr(0, lastSlash + 1) + newName;
    
    return PerformFileOperation(FO_RENAME, filePath, newPath.c_str(), flags);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::MoveItem(PCWSTR oldPath, PCWSTR newFolder) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_MOVE, oldPath, newFolder, 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::MoveItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_MOVE, oldPath, newFolder, flags);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::MoveItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr || name == nullptr)
    {
        return E_INVALIDARG;
    }
    
    // Construct new path with new name
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_MOVE, oldPath, newPath.c_str(), 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::MoveItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr || name == nullptr)
    {
        return E_INVALIDARG;
    }
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_MOVE, oldPath, newPath.c_str(), flags);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CopyItem(PCWSTR oldPath, PCWSTR newFolder) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_COPY, oldPath, newFolder, 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CopyItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_COPY, oldPath, newFolder, flags);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CopyItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr || name == nullptr)
    {
        return E_INVALIDARG;
    }
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_COPY, oldPath, newPath.c_str(), 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CopyItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept
{
    if (oldPath == nullptr || newFolder == nullptr || name == nullptr)
    {
        return E_INVALIDARG;
    }
    
    std::wstring newPath = std::wstring(newFolder);
    if (!newPath.empty() && newPath.back() != L'\\' && newPath.back() != L'/')
    {
        newPath += L'\\';
    }
    newPath += name;
    
    return PerformFileOperation(FO_COPY, oldPath, newPath.c_str(), flags);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::DeleteItem(PCWSTR filePath) noexcept
{
    if (filePath == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_DELETE, filePath, nullptr, 0);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::DeleteItemWithOptions(PCWSTR filePath, long flags) noexcept
{
    if (filePath == nullptr)
    {
        return E_INVALIDARG;
    }
    
    return PerformFileOperation(FO_DELETE, filePath, nullptr, flags);
}

// IHutaoNativeFileSystem2 methods
HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CreateLink(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName) noexcept
{
    if (fileLocation == nullptr || fileName == nullptr)
    {
        return E_INVALIDARG;
    }
    
    // Create IShellLink object
    com_ptr<IShellLinkW> shellLink;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Set path
    hr = shellLink->SetPath(fileLocation);
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Set arguments if provided
    if (arguments != nullptr && wcslen(arguments) > 0)
    {
        hr = shellLink->SetArguments(arguments);
        if (FAILED(hr))
        {
            return hr;
        }
    }
    
    // Set icon location if provided
    if (iconLocation != nullptr && wcslen(iconLocation) > 0)
    {
        hr = shellLink->SetIconLocation(iconLocation, 0);
        if (FAILED(hr))
        {
            return hr;
        }
    }
    
    // Get IPersistFile interface
    com_ptr<IPersistFile> persistFile = shellLink.as<IPersistFile>();
    
    // Save the link
    hr = persistFile->Save(fileName, TRUE);
    if (FAILED(hr))
    {
        return hr;
    }
    
    return S_OK;
}

// IHutaoNativeFileSystem3 methods
HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::PickFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept
{
    if (picked == nullptr || path == nullptr)
    {
        return E_POINTER;
    }
    
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
        return CreateHutaoStringFromWideString(fileBuffer, path);
    }
    
    // Check for cancellation
    DWORD error = CommDlgExtendedError();
    if (error == 0)
    {
        // User cancelled
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(error);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::SaveFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept
{
    if (picked == nullptr || path == nullptr)
    {
        return E_POINTER;
    }
    
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
        return CreateHutaoStringFromWideString(fileBuffer, path);
    }
    
    // Check for cancellation
    DWORD error = CommDlgExtendedError();
    if (error == 0)
    {
        // User cancelled
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(error);
}

HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::PickFolder(HWND hwnd, PCWSTR title, BOOL* picked, IHutaoString** path) noexcept
{
    if (picked == nullptr || path == nullptr)
    {
        return E_POINTER;
    }
    
    *picked = FALSE;
    *path = nullptr;
    
    // Initialize COM for BROWSEINFO
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
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
            hr = CreateHutaoStringFromWideString(folderPath, path);
        }
        
        // Free PIDL
        CoTaskMemFree(pidl);
    }
    
    CoUninitialize();
    
    return hr;
}

// IHutaoNativeFileSystem4 methods
HRESULT STDMETHODCALLTYPE HutaoNativeFileSystem::CopyFileAllowDecryptedDestination(PCWSTR existingFileName, PCWSTR newFileName, BOOL overwrite) noexcept
{
    if (existingFileName == nullptr || newFileName == nullptr)
    {
        return E_INVALIDARG;
    }
    
    // Use CopyFileExW with COPY_FILE_ALLOW_DECRYPTED_DESTINATION flag
    DWORD flags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION;
    if (!overwrite)
    {
        flags |= COPY_FILE_FAIL_IF_EXISTS;
    }
    
    BOOL result = CopyFileExW(existingFileName, newFileName, nullptr, nullptr, nullptr, flags);
    if (!result)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    return S_OK;
}
