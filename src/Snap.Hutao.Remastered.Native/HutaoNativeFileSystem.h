#pragma once
#include "IHutaoNativeFileSystem_h.h"
#include "IHutaoNativeFileSystem2_h.h"
#include "IHutaoNativeFileSystem3_h.h"
#include "IHutaoNativeFileSystem4_h.h"
#include "IHutaoString_h.h"
#include <winrt/base.h>
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <string>

class HutaoNativeFileSystem : public winrt::implements<HutaoNativeFileSystem, 
    IHutaoNativeFileSystem, 
    IHutaoNativeFileSystem2, 
    IHutaoNativeFileSystem3, 
    IHutaoNativeFileSystem4, 
    winrt::non_agile>
{
public:
    HutaoNativeFileSystem() = default;
    ~HutaoNativeFileSystem() = default;

    // IHutaoNativeFileSystem methods
    HRESULT STDMETHODCALLTYPE RenameItem(PCWSTR filePath, PCWSTR newName) noexcept override;
    HRESULT STDMETHODCALLTYPE RenameItemWithOptions(PCWSTR filePath, PCWSTR newName, long flags) noexcept override;
    HRESULT STDMETHODCALLTYPE MoveItem(PCWSTR oldPath, PCWSTR newFolder) noexcept override;
    HRESULT STDMETHODCALLTYPE MoveItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept override;
    HRESULT STDMETHODCALLTYPE MoveItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept override;
    HRESULT STDMETHODCALLTYPE MoveItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept override;
    HRESULT STDMETHODCALLTYPE CopyItem(PCWSTR oldPath, PCWSTR newFolder) noexcept override;
    HRESULT STDMETHODCALLTYPE CopyItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept override;
    HRESULT STDMETHODCALLTYPE CopyItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept override;
    HRESULT STDMETHODCALLTYPE CopyItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept override;
    HRESULT STDMETHODCALLTYPE DeleteItem(PCWSTR filePath) noexcept override;
    HRESULT STDMETHODCALLTYPE DeleteItemWithOptions(PCWSTR filePath, long flags) noexcept override;

    // IHutaoNativeFileSystem2 methods
    HRESULT STDMETHODCALLTYPE CreateLink(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName) noexcept override;

    // IHutaoNativeFileSystem3 methods
    HRESULT STDMETHODCALLTYPE PickFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept override;
    HRESULT STDMETHODCALLTYPE SaveFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept override;
    HRESULT STDMETHODCALLTYPE PickFolder(HWND hwnd, PCWSTR title, BOOL* picked, IHutaoString** path) noexcept override;

    // IHutaoNativeFileSystem4 methods
    HRESULT STDMETHODCALLTYPE CopyFileAllowDecryptedDestination(PCWSTR existingFileName, PCWSTR newFileName, BOOL overwrite) noexcept override;

private:
    static HRESULT PerformFileOperation(UINT operation, PCWSTR source, PCWSTR destination, long flags);
    static HRESULT CreateHutaoStringFromWideString(PCWSTR wideString, IHutaoString** ppString);
};
