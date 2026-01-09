#include "pch.h"
#include "SimpleTestFramework.h"
#include "HutaoNativeImports.h"
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <sstream>

// Empty window subclass callback
static BOOL __stdcall EmptyWindowSubclassCallback(HWND, UINT, WPARAM, LPARAM, GCHandle, LRESULT*) { return TRUE; }

// 控制台输出辅助函数
void WriteToConsole(const std::wstring& message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE || hConsole == NULL) {
        // 尝试获取控制台句柄失败，使用AllocConsole
        if (AllocConsole()) {
            hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        }
    }
    
    if (hConsole != INVALID_HANDLE_VALUE && hConsole != NULL) {
        DWORD charsWritten;
        WriteConsoleW(hConsole, message.c_str(), (DWORD)message.length(), &charsWritten, NULL);
    }
}

void WriteToConsole(const std::string& message) {
    // 将UTF-8字符串转换为宽字符串
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), (int)message.length(), NULL, 0);
    if (size_needed > 0) {
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, message.c_str(), (int)message.length(), &wstr[0], size_needed);
        WriteToConsole(wstr);
    }
}

using namespace SimpleTestFramework;
using namespace winrt;
using namespace Windows::Foundation;

// ============================================================================
// 基础功能测试用例
// ============================================================================

TEST_CASE(TestBasicAssertions, "测试基本断言功能")
{
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
    ASSERT_EQUAL(1, 1);
    ASSERT_NOT_EQUAL(1, 2);
    
    int x = 5;
    int* ptr = &x;
    ASSERT_NOT_NULL(ptr);
    ASSERT_NULL(nullptr);
}

TEST_CASE(TestHResultAssertions, "测试HRESULT断言功能")
{
    HRESULT hrSuccess = S_OK;
    HRESULT hrFailure = E_FAIL;
    
    ASSERT_SUCCEEDED(hrSuccess);
    ASSERT_FAILED(hrFailure);
}

// ============================================================================
// IHutaoNative接口测试用例
// ============================================================================

TEST_CASE(TestIHutaoNativeInterface, "测试IHutaoNative接口功能")
{
    // Create IHutaoNative instance
    IHutaoNative* pNative = nullptr;
    HRESULT hr = MakeHutaoNative(&pNative);
    ASSERT_SUCCEEDED(hr);
    ASSERT_NOT_NULL(pNative);
    
    // Test LoopbackSupport
    IHutaoNativeLoopbackSupport* pLoopbackSupport = nullptr;
    hr = pNative->MakeLoopbackSupport(&pLoopbackSupport);
    if (SUCCEEDED(hr) && pLoopbackSupport) {
        pLoopbackSupport->Release();
    }
    
    // Test RegistryNotification
    IHutaoNativeRegistryNotification* pRegistryNotification = nullptr;
    hr = pNative->MakeRegistryNotification(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", &pRegistryNotification);
    if (SUCCEEDED(hr) && pRegistryNotification) {
        pRegistryNotification->Release();
    }
    
    // Test WindowSubclass
    IHutaoNativeWindowSubclass* pWindowSubclass = nullptr;
    HutaoNativeWindowSubclassCallback callback = HutaoNativeWindowSubclassCallback::Create(EmptyWindowSubclassCallback);
    GCHandle userData = GCHandle(); // Empty GCHandle
    hr = pNative->MakeWindowSubclass(nullptr, callback, userData, &pWindowSubclass);
    if (SUCCEEDED(hr) && pWindowSubclass) {
        pWindowSubclass->Release();
    }
    
    // Test WindowNonRude
    IHutaoNativeWindowNonRude* pWindowNonRude = nullptr;
    hr = pNative->MakeWindowNonRude(nullptr, &pWindowNonRude);
    if (SUCCEEDED(hr) && pWindowNonRude) {
        pWindowNonRude->Release();
    }
    
    // Cleanup
    pNative->Release();
}

TEST_CASE(TestIHutaoNative2Interface, "测试IHutaoNative2接口功能")
{
    // Create IHutaoNative2 instance
    IHutaoNative2* pNative2 = nullptr;
    HRESULT hr = MakeHutaoNative2(&pNative2);
    ASSERT_SUCCEEDED(hr);
    ASSERT_NOT_NULL(pNative2);
    
    // Test DeviceCapabilities
    IHutaoNativeDeviceCapabilities* pDeviceCapabilities = nullptr;
    hr = pNative2->MakeDeviceCapabilities(&pDeviceCapabilities);
    if (SUCCEEDED(hr) && pDeviceCapabilities) {
        pDeviceCapabilities->Release();
    }
    
    // Test PhysicalDrive
    IHutaoNativePhysicalDrive* pPhysicalDrive = nullptr;
    hr = pNative2->MakePhysicalDrive(&pPhysicalDrive);
    if (SUCCEEDED(hr) && pPhysicalDrive) {
        pPhysicalDrive->Release();
    }
    
    // Test LogicalDrive
    IHutaoNativeLogicalDrive* pLogicalDrive = nullptr;
    hr = pNative2->MakeLogicalDrive(&pLogicalDrive);
    if (SUCCEEDED(hr) && pLogicalDrive) {
        pLogicalDrive->Release();
    }
    
    // Cleanup
    pNative2->Release();
}

TEST_CASE(TestIHutaoNative3Interface, "测试IHutaoNative3接口功能")
{
    // Create IHutaoNative3 instance
    IHutaoNative3* pNative3 = nullptr;
    HRESULT hr = MakeHutaoNative3(&pNative3);
    ASSERT_SUCCEEDED(hr);
    ASSERT_NOT_NULL(pNative3);
    
    // Test InputLowLevelKeyboardSource
    IHutaoNativeInputLowLevelKeyboardSource* pKeyboardSource = nullptr;
    hr = pNative3->MakeInputLowLevelKeyboardSource(&pKeyboardSource);
    if (SUCCEEDED(hr) && pKeyboardSource) {
        pKeyboardSource->Release();
    }
    
    // Cleanup
    pNative3->Release();
}

TEST_CASE(TestIHutaoNative4Interface, "测试IHutaoNative4接口功能")
{
    // Create IHutaoNative4 instance
    IHutaoNative4* pNative4 = nullptr;
    HRESULT hr = MakeHutaoNative4(&pNative4);
    ASSERT_SUCCEEDED(hr);
    ASSERT_NOT_NULL(pNative4);
    
    // Test FileSystem
    IHutaoNativeFileSystem* pFileSystem = nullptr;
    hr = pNative4->MakeFileSystem(&pFileSystem);
    
    if (SUCCEEDED(hr) && pFileSystem) {
        // Test FileSystem2 interface
        IHutaoNativeFileSystem2* pFileSystem2 = nullptr;
        hr = pFileSystem->QueryInterface(__uuidof(IHutaoNativeFileSystem2), (void**)&pFileSystem2);
        if (SUCCEEDED(hr) && pFileSystem2) {
            pFileSystem2->Release();
        }
        
        // Test FileSystem3 interface
        IHutaoNativeFileSystem3* pFileSystem3 = nullptr;
        hr = pFileSystem->QueryInterface(__uuidof(IHutaoNativeFileSystem3), (void**)&pFileSystem3);
        if (SUCCEEDED(hr) && pFileSystem3) {
            pFileSystem3->Release();
        }
        
        // Test FileSystem4 interface
        IHutaoNativeFileSystem4* pFileSystem4 = nullptr;
        hr = pFileSystem->QueryInterface(__uuidof(IHutaoNativeFileSystem4), (void**)&pFileSystem4);
        if (SUCCEEDED(hr) && pFileSystem4) {
            pFileSystem4->Release();
        }
        
        pFileSystem->Release();
    }
    
    // Cleanup
    pNative4->Release();
}

TEST_CASE(TestIHutaoNative5Interface, "测试IHutaoNative5接口功能")
{
    // Create IHutaoNative5 instance
    IHutaoNative5* pNative5 = nullptr;
    HRESULT hr = MakeHutaoNative5(&pNative5);
    ASSERT_SUCCEEDED(hr);
    ASSERT_NOT_NULL(pNative5);
    
    // Test NotifyIcon
    IHutaoNativeNotifyIcon* pNotifyIcon = nullptr;
    GUID notifyIconId = {0};
    hr = pNative5->MakeNotifyIcon(L"", &notifyIconId, &pNotifyIcon);
    if (SUCCEEDED(hr) && pNotifyIcon) {
        pNotifyIcon->Release();
    }
    
    // Cleanup
    pNative5->Release();
}

// ============================================================================
// 测试套件定义和注册
// ============================================================================

// 基础功能测试套件
TEST_SUITE(BasicFunctionTests, "基础功能测试套件")

// 接口功能测试套件
TEST_SUITE(InterfaceTests, "接口功能测试套件")

// 注册测试用例到套件
class TestSuiteRegistrar {
public:
    TestSuiteRegistrar() {
        // 基础功能测试套件
        auto basicSuite = std::make_shared<TestSuite>(L"BasicFunctionTests", L"基础功能测试套件");
        basicSuite->AddTestCase(std::make_shared<TestBasicAssertions_TestCase>());
        basicSuite->AddTestCase(std::make_shared<TestHResultAssertions_TestCase>());
        
        // 接口功能测试套件
        auto interfaceSuite = std::make_shared<TestSuite>(L"InterfaceTests", L"接口功能测试套件");
        interfaceSuite->AddTestCase(std::make_shared<TestIHutaoNativeInterface_TestCase>());
        interfaceSuite->AddTestCase(std::make_shared<TestIHutaoNative2Interface_TestCase>());
        interfaceSuite->AddTestCase(std::make_shared<TestIHutaoNative3Interface_TestCase>());
        interfaceSuite->AddTestCase(std::make_shared<TestIHutaoNative4Interface_TestCase>());
        interfaceSuite->AddTestCase(std::make_shared<TestIHutaoNative5Interface_TestCase>());
        
        TestRunner& runner = TestRunner::GetInstance();
        runner.AddTestSuite(basicSuite);
        runner.AddTestSuite(interfaceSuite);
    }
};

static TestSuiteRegistrar testSuiteRegistrar;

// ============================================================================
// 主函数
// ============================================================================

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 使用WriteToConsole输出
    WriteToConsole(L"\n");
    WriteToConsole(L"═══════════════════════════════════════════════════════════════\n");
    WriteToConsole(L"           Snap.Hutao.Remastered.Native 测试框架\n");
    WriteToConsole(L"═══════════════════════════════════════════════════════════════\n\n");
    
    WriteToConsole(L"测试套件:\n");
    WriteToConsole(L"  1. 基础功能测试套件 (2个测试用例)\n");
    WriteToConsole(L"  2. 接口功能测试套件 (5个接口测试用例)\n");
    WriteToConsole(L"  总计: 7个测试用例\n\n");

    // 配置测试运行器
    TestRunner& runner = TestRunner::GetInstance();
    runner.SetStopOnFirstFailure(false);
    runner.SetReportFormat(L"html");

    // 运行所有测试
    bool allPassed = runner.RunAllTests();

    // 生成并保存报告
    WriteToConsole(L"\n=== 生成测试报告 ===\n");

    // 保存文本报告
    runner.SaveReportToFile(L"test_report.txt", L"text");

    // 保存HTML报告
    runner.SaveReportToFile(L"test_report.html", L"html");

    // 保存XML报告
    runner.SaveReportToFile(L"test_report.xml", L"xml");

    WriteToConsole(L"\n═══════════════════════════════════════════════════════════════\n");

    if (allPassed) {
        WriteToConsole(L"🎉 所有测试通过！测试框架运行成功。\n");
    }
    else {
        WriteToConsole(L"❌ 有测试失败！请检查详细报告。\n");
    }

    // 等待用户按任意键（仅用于调试）
    WriteToConsole(L"\n按任意键继续...\n");
    _getch();

    return allPassed ? 0 : 1;
}
