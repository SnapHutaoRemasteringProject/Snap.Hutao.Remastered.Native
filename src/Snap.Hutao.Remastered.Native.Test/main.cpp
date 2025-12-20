#include "pch.h"
#include "HutaoNativeImports.h"

using namespace winrt;
using namespace Windows::Foundation;

void __stdcall HutaoNativeLoggingCallbackImpl(FailureInfo* info)
{
    printf("HutaoNativeLoggingCallback invoked. HR: 0x%08X\n", info->hr);
}

void __stdcall HutaoNativeMessageCallbackImpl(FailureInfo* info, PWSTR pszDebugMessage, ULONG64 cchDebugMessage)
{
    printf("HutaoNativeMessageCallback invoked. Message: %ls\n", pszDebugMessage);
}

int main()
{
    init_apartment();

    HutaoInitializeWilCallbacks({ &HutaoNativeLoggingCallbackImpl }, {&HutaoNativeMessageCallbackImpl});
    HutaoTestWilCallbacks();

	IHutaoNative* native = nullptr;
	HutaoCreateInstance(&native);
    Uri uri(L"http://aka.ms/cppwinrt");
    printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
}
