// Provide a minimal stub for WINRT_IMPL_RoOriginateLanguageException to satisfy linker
// This file intentionally does not include <winrt/base.h> to avoid duplicate implementations.
extern "C" void WINRT_IMPL_RoOriginateLanguageException(...)
{
    // no-op
}

// Ensure runtimeobject is linked for WindowsGetStringRawBuffer
#pragma comment(lib, "runtimeobject.lib")
