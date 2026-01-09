#pragma once
#include "IHutaoNativeFileSystem.h"
#include "IHutaoNativeFileSystem2.h"
#include "IHutaoNativeFileSystem3.h"
#include "IHutaoNativeFileSystem4.h"
#include "IHutaoString.h"
#include "CustomImplements.h"
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>
#include <string>

class HutaoNativeFileSystem : public hutao::CustomImplements<HutaoNativeFileSystem, 
    IHutaoNativeFileSystem, 
    IHutaoNativeFileSystem2, 
    IHutaoNativeFileSystem3, 
    IHutaoNativeFileSystem4>
{
public:
    HutaoNativeFileSystem() = default;
    ~HutaoNativeFileSystem() = default;

    // IHutaoNativeFileSystem methods
    HRESULT __stdcall RenameItem(PCWSTR filePath, PCWSTR newName) noexcept override;
    HRESULT __stdcall RenameItemWithOptions(PCWSTR filePath, PCWSTR newName, long flags) noexcept override;
    HRESULT __stdcall MoveItem(PCWSTR oldPath, PCWSTR newFolder) noexcept override;
    HRESULT __stdcall MoveItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept override;
    HRESULT __stdcall MoveItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept override;
    HRESULT __stdcall MoveItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept override;
    HRESULT __stdcall CopyItem(PCWSTR oldPath, PCWSTR newFolder) noexcept override;
    HRESULT __stdcall CopyItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) noexcept override;
    HRESULT __stdcall CopyItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) noexcept override;
    HRESULT __stdcall CopyItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) noexcept override;
    HRESULT __stdcall DeleteItem(PCWSTR filePath) noexcept override;
    HRESULT __stdcall DeleteItemWithOptions(PCWSTR filePath, long flags) noexcept override;

    // IHutaoNativeFileSystem2 methods
    HRESULT __stdcall CreateLink(PCWSTR fileLocation, PCWSTR arguments, PCWSTR iconLocation, PCWSTR fileName) noexcept override;

    // IHutaoNativeFileSystem3 methods
    HRESULT __stdcall PickFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept override;
    HRESULT __stdcall SaveFile(HWND hwnd, PCWSTR title, PCWSTR defaultFileName, PCWSTR fileFilterName, PCWSTR fileFilterType, BOOL* picked, IHutaoString** path) noexcept override;
    HRESULT __stdcall PickFolder(HWND hwnd, PCWSTR title, BOOL* picked, IHutaoString** path) noexcept override;

    // IHutaoNativeFileSystem4 methods
    HRESULT __stdcall CopyFileAllowDecryptedDestination(PCWSTR existingFileName, PCWSTR newFileName, BOOL overwrite) noexcept override;

private:
    static HRESULT PerformFileOperation(UINT operation, PCWSTR source, PCWSTR destination, long flags);
    static HRESULT CreateHutaoStringFromWideString(PCWSTR wideString, IHutaoString** ppString);
};
