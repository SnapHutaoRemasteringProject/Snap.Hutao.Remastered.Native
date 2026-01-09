#pragma once
#include <Windows.h>
#include <unknwn.h>

// {FDD58117-0C7F-44D8-A7A2-8B1766474A93}
class __declspec(uuid("FDD58117-0C7F-44D8-A7A2-8B1766474A93")) IHutaoNativeFileSystem : public IUnknown
{
public:
    virtual HRESULT __stdcall RenameItem(PCWSTR filePath, PCWSTR newName) = 0;
    virtual HRESULT __stdcall RenameItemWithOptions(PCWSTR filePath, PCWSTR newName, long flags) = 0;
    virtual HRESULT __stdcall MoveItem(PCWSTR oldPath, PCWSTR newFolder) = 0;
    virtual HRESULT __stdcall MoveItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) = 0;
    virtual HRESULT __stdcall MoveItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) = 0;
    virtual HRESULT __stdcall MoveItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) = 0;
    virtual HRESULT __stdcall CopyItem(PCWSTR oldPath, PCWSTR newFolder) = 0;
    virtual HRESULT __stdcall CopyItemWithOptions(PCWSTR oldPath, PCWSTR newFolder, long flags) = 0;
    virtual HRESULT __stdcall CopyItemWithName(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name) = 0;
    virtual HRESULT __stdcall CopyItemWithNameAndOptions(PCWSTR oldPath, PCWSTR newFolder, PCWSTR name, long flags) = 0;
    virtual HRESULT __stdcall DeleteItem(PCWSTR filePath) = 0;
    virtual HRESULT __stdcall DeleteItemWithOptions(PCWSTR filePath, long flags) = 0;
};
