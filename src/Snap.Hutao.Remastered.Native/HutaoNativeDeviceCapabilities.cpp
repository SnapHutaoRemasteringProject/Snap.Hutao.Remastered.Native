#include "HutaoNativeDeviceCapabilities.h"
#include <Windows.h>

HRESULT STDMETHODCALLTYPE HutaoNativeDeviceCapabilities::GetPrimaryScreenVerticalRefreshRate(int* refreshRate) noexcept
{
    if (refreshRate == nullptr)
    {
        return E_POINTER;
    }

    // 获取主显示器的刷新率
    HDC hdc = GetDC(nullptr);
    if (hdc == nullptr)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    int refresh = GetDeviceCaps(hdc, VREFRESH);
    ReleaseDC(nullptr, hdc);

    // GetDeviceCaps返回的是垂直刷新率（以赫兹为单位）
    // 如果返回0或1，可能表示无法获取刷新率
    if (refresh <= 1)
    {
        // 默认返回60Hz
        *refreshRate = 60;
    }
    else
    {
        *refreshRate = refresh;
    }

    return S_OK;
}
