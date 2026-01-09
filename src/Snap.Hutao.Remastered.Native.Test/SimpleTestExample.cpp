#include "pch.h"
#include "SimpleTestFramework.h"
#include <windows.h>

using namespace SimpleTestFramework;

// 测试用例1：基本断言测试
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

// 测试用例2：HRESULT测试
TEST_CASE(TestHResultAssertions, "测试HRESULT断言功能")
{
    HRESULT hrSuccess = S_OK;
    HRESULT hrFailure = E_FAIL;
    
    ASSERT_SUCCEEDED(hrSuccess);
    ASSERT_FAILED(hrFailure);
}

// 测试用例3：字符串测试
TEST_CASE(TestStringAssertions, "测试字符串断言功能")
{
    std::string str1 = "hello";
    std::string str2 = "hello";
    std::string str3 = "world";
    
    ASSERT_EQUAL(str1, str2);
    ASSERT_NOT_EQUAL(str1, str3);
}

// 测试用例4：异常测试
TEST_CASE(TestExceptionAssertions, "测试异常断言功能")
{
    // 测试抛出异常
    ASSERT_THROWS([]() {
        throw std::runtime_error("test exception");
    });
    
    // 测试不抛出异常
    ASSERT_NO_THROW([]() {
        int x = 1 + 1;
    });
}

// 测试套件：基础功能测试
TEST_SUITE(BasicFunctionTests, "基础功能测试套件")

// 注册测试用例到套件
class BasicTestSuiteRegistrar {
public:
    BasicTestSuiteRegistrar() {
        auto suite = std::make_shared<TestSuite>("BasicFunctionTests", "基础功能测试套件");
        
        // 创建测试用例实例并注册
        suite->AddTestCase(std::make_shared<TestBasicAssertions_TestCase>());
        suite->AddTestCase(std::make_shared<TestHResultAssertions_TestCase>());
        suite->AddTestCase(std::make_shared<TestStringAssertions_TestCase>());
        suite->AddTestCase(std::make_shared<TestExceptionAssertions_TestCase>());
        
        TestRunner::GetInstance().AddTestSuite(suite);
    }
};

static BasicTestSuiteRegistrar basicTestSuiteRegistrar;

// 主函数 - 运行所有测试
int RunSimpleTests()
{
    std::wcout << L"=== 开始运行简易测试框架示例 ===\n";
    
    // 配置测试运行器
    TestRunner& runner = TestRunner::GetInstance();
    runner.SetStopOnFirstFailure(false);
    
    // 运行所有测试
    bool allPassed = runner.RunAllTests();
    
    // 生成报告
    std::string report = runner.GenerateReport();
    std::wcout << L"\n=== 文本报告 ===\n";
    std::cout << report << std::endl;
    
    // 保存HTML报告
    runner.SaveReportToFile("test_report.html", "html");
    
    // 保存XML报告
    runner.SaveReportToFile("test_report.xml", "xml");
    
    if (allPassed) {
        std::wcout << L"\n✅ 所有测试通过！\n";
        return 0;
    } else {
        std::wcout << L"\n❌ 有测试失败！\n";
        return 1;
    }
}

// 集成到现有main函数的辅助函数
extern "C" __declspec(dllexport) int RunSimpleTestFramework()
{
    return RunSimpleTests();
}
