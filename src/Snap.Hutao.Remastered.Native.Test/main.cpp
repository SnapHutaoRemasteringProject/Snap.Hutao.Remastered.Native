#include "pch.h"
#include "HutaoNativeImports.h"

using namespace winrt;
using namespace Windows::Foundation;

int main()
{
    init_apartment();
	IHutaoNative* native = nullptr;
	HutaoCreateInstance(&native);
    Uri uri(L"http://aka.ms/cppwinrt");
    printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
}
