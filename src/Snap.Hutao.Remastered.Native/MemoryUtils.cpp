#include "MemoryUtils.h"

// 向目标进程写入字符串并返回远程内存地址
BOOL WriteProcessStringW(HANDLE hProcess, LPCWSTR str, LPVOID* ppRemoteMem)
{
    if (!hProcess || !str || !ppRemoteMem) {
        return FALSE;
    }

    // 计算字符串长度（包含终止符）
    SIZE_T strLen = (wcslen(str) + 1) * sizeof(WCHAR);

    // 在目标进程中分配内存
    LPVOID pRemoteMem = VirtualAllocEx(hProcess, NULL, strLen,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!pRemoteMem) {
        return FALSE;
    }

    // 写入字符串到目标进程
    SIZE_T bytesWritten = 0;
    BOOL success = WriteProcessMemory(hProcess, pRemoteMem, str, strLen, &bytesWritten);

    if (!success || bytesWritten != strLen) {
        VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }

    *ppRemoteMem = pRemoteMem;
    return TRUE;
}

// 向目标进程写入数据
BOOL WriteProcessData(HANDLE hProcess, LPVOID pRemoteAddress, LPCVOID pData, SIZE_T dataSize)
{
    if (!hProcess || !pRemoteAddress || !pData || dataSize == 0) {
        return FALSE;
    }

    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(hProcess, pRemoteAddress, pData, dataSize, &bytesWritten) &&
        bytesWritten == dataSize;
}

// 从目标进程读取数据
BOOL ReadProcessData(HANDLE hProcess, LPCVOID pRemoteAddress, LPVOID pBuffer, SIZE_T bufferSize)
{
    if (!hProcess || !pRemoteAddress || !pBuffer || bufferSize == 0) {
        return FALSE;
    }

    SIZE_T bytesRead = 0;
    return ReadProcessMemory(hProcess, pRemoteAddress, pBuffer, bufferSize, &bytesRead) &&
        bytesRead == bufferSize;
}

// 创建注入数据结构
PINJECTION_DATA CreateInjectionData(LPCWSTR dllPath, LPCWSTR functionName)
{
    PINJECTION_DATA pData = (PINJECTION_DATA)malloc(sizeof(INJECTION_DATA));
    if (!pData) {
        return NULL;
    }

    wcsncpy_s(pData->dllPath, MAX_PATH, dllPath, _TRUNCATE);
    wcsncpy_s(pData->functionName, 256, functionName, _TRUNCATE);

    return pData;
}

// 释放注入数据结构
VOID FreeInjectionData(PINJECTION_DATA pData)
{
    if (pData) {
        free(pData);
    }
}

// Memory utilities patch function
DLL_EXPORT HRESULT __stdcall MemoryUtilitiesPatch(
    PCWSTR moduleName, 
    uint offset, 
    int size, 
    PatchCallbackFunc callback, 
    GCHandle state)
{
    if (!moduleName || size <= 0 || !callback) {
        return E_INVALIDARG;
    }

    // 获取模块句柄
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (!hModule) {
        // 如果模块未加载，尝试加载它
        hModule = LoadLibraryW(moduleName);
        if (!hModule) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }

    // 计算要修补的内存地址
    byte* patchAddress = reinterpret_cast<byte*>(hModule) + offset;

    // 分配临时缓冲区来存储原始数据
    byte* originalData = new byte[size];
    if (!originalData) {
        return E_OUTOFMEMORY;
    }

    // 保存原始内存保护
    DWORD oldProtect = 0;
    
    // 修改内存保护为可读写执行
    if (!VirtualProtect(patchAddress, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        delete[] originalData;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = S_OK;
    
    try {
        // 调用回调函数，传递内存地址和大小
        hr = callback(patchAddress, size, state);
        
        if (FAILED(hr)) {
            // 如果回调失败，恢复原始内存保护
            VirtualProtect(patchAddress, size, oldProtect, &oldProtect);
        }
    }
    catch (...) {
        // 捕获任何异常并恢复内存保护
        VirtualProtect(patchAddress, size, oldProtect, &oldProtect);
        delete[] originalData;
        return E_UNEXPECTED;
    }

    // 恢复原始内存保护
    DWORD tempProtect = 0;
    VirtualProtect(patchAddress, size, oldProtect, &tempProtect);

    delete[] originalData;
    return hr;
}
