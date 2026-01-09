#pragma once
#include <Windows.h>
#include <unknwn.h>

// {FB4825F4-3A7F-47A1-8923-32933CB7DE92}
class __declspec(uuid("FB4825F4-3A7F-47A1-8923-32933CB7DE92")) IHutaoNativeFileSystem4 : public IUnknown
{
public:
    virtual HRESULT __stdcall CopyFileAllowDecryptedDestination(PCWSTR existingFileName, PCWSTR newFileName, BOOL overwrite) = 0;
};
