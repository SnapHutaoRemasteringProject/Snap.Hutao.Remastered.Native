#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>

// 控制台输出辅助函数
void WriteToConsole(const std::wstring& message);
void WriteToConsole(const std::string& message);

namespace SimpleTestFramework {

#undef ERROR
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

// 测试用例信息
struct TestCaseInfo {
    std::wstring name;
    std::wstring description;
    std::wstring file;
    int line;
    TestResult result;
    std::wstring message;
    double duration;
    
    TestCaseInfo() : line(0), result(TestResult::SKIPPED), duration(0.0) {}
};

// 测试套件信息
struct TestSuiteInfo {
    std::wstring name;
    std::wstring description;
    std::vector<TestCaseInfo> testCases;
    int totalTests;
    int passedTests;
    int failedTests;
    int skippedTests;
    int errorTests;
    double totalDuration;
    
    TestSuiteInfo() : totalTests(0), passedTests(0), failedTests(0), 
                     skippedTests(0), errorTests(0), totalDuration(0.0) {}
};

// 测试断言类
class TestAssert {
public:
    static void IsTrue(bool condition, const std::wstring& message = L"", 
                      const char* file = __FILE__, int line = __LINE__);
    
    static void IsFalse(bool condition, const std::wstring& message = L"", 
                       const char* file = __FILE__, int line = __LINE__);
    
    static void AreEqual(int expected, int actual, const std::wstring& message = L"", 
                        const char* file = __FILE__, int line = __LINE__);
    
    static void AreEqual(const std::wstring& expected, const std::wstring& actual, 
                        const std::wstring& message = L"", const char* file = __FILE__, 
                        int line = __LINE__);
    
    static void AreEqual(const std::string& expected, const std::string& actual, 
                        const std::wstring& message = L"", const char* file = __FILE__, 
                        int line = __LINE__);
    
    static void AreNotEqual(int expected, int actual, const std::wstring& message = L"", 
                           const char* file = __FILE__, int line = __LINE__);
    static void Succeeded(HRESULT hr, const std::wstring& message = L"", 
                         const char* file = __FILE__, int line = __LINE__);
    
    static void Failed(HRESULT hr, const std::wstring& message = L"", 
                      const char* file = __FILE__, int line = __LINE__);
    static void IsNull(void* ptr, const std::wstring& message = L"", 
                      const char* file = __FILE__, int line = __LINE__);
    
    static void IsNotNull(void* ptr, const std::wstring& message = L"", 
                         const char* file = __FILE__, int line = __LINE__);
    static void Throws(const std::function<void()>& func, const std::wstring& message = L"", 
                      const char* file = __FILE__, int line = __LINE__);
    
    static void NoThrow(const std::function<void()>& func, const std::wstring& message = L"", 
                       const char* file = __FILE__, int line = __LINE__);
};

// 测试用例基类
class TestCase {
protected:
    TestCaseInfo m_info;
    std::wstringstream m_failureMessage;
    bool m_hasFailed;
    
public:
    TestCase(const std::wstring& name, const std::wstring& description = L"");
    virtual ~TestCase() = default;
    
    virtual void SetUp();
    virtual void RunTest() = 0;
    virtual void TearDown();
    
    TestResult Execute();
    
    const TestCaseInfo& GetInfo() const { return m_info; }
    void SetFileInfo(const wchar_t* file, int line);
    
protected:
    void RecordFailure(const std::wstring& message, const wchar_t* file, int line);
};

// 测试套件类
class TestSuite {
private:
    TestSuiteInfo m_info;
    std::vector<std::shared_ptr<TestCase>> m_testCases;
    
public:
    TestSuite(const std::wstring& name, const std::wstring& description = L"");
    
    void AddTestCase(std::shared_ptr<TestCase> testCase);
    TestSuiteInfo Run();
    const TestSuiteInfo& GetInfo() const { return m_info; }

    void Clear();
};

// 测试运行器（单例）
class TestRunner {
private:
    static TestRunner* s_instance;
    std::vector<std::shared_ptr<TestSuite>> m_testSuites;
    bool m_stopOnFirstFailure;
    std::wstring m_reportOutputPath;
    std::wstring m_reportFormat;
    
    TestRunner();
    
public:
    ~TestRunner();

    static TestRunner& GetInstance();
    void AddTestSuite(std::shared_ptr<TestSuite> testSuite);
    bool RunAllTests();
    bool RunTestSuite(const std::wstring& suiteName);
    void SetStopOnFirstFailure(bool stop) { m_stopOnFirstFailure = stop; }
    
    // 报告相关方法
    std::wstring GenerateReport() const;
    std::wstring GenerateHtmlReport() const;
    std::wstring GenerateXmlReport() const;
    bool SaveReportToFile(const std::wstring& filePath, const std::wstring& format = L"text") const;
    
    // 配置方法
    void SetReportOutputPath(const std::wstring& path) { m_reportOutputPath = path; }
    void SetReportFormat(const std::wstring& format) { m_reportFormat = format; }
    
    void ClearAllTests();
    
    struct TestStatistics {
        int totalSuites;
        int totalTests;
        int passedTests;
        int failedTests;
        int skippedTests;
        int errorTests;
        double totalDuration;
    };
    
    TestStatistics GetStatistics() const;
};

#define TEST_CASE(testName, description) \
    class testName##_TestCase : public SimpleTestFramework::TestCase { \
    public: \
        testName##_TestCase() : SimpleTestFramework::TestCase(L## #testName, L## description) {} \
        void RunTest() override; \
    }; \
    static testName##_TestCase testName##_Instance; \
    void testName##_TestCase::RunTest()

#define TEST_SUITE(suiteName, description) \
    class suiteName##_TestSuite { \
    public: \
        suiteName##_TestSuite() { \
            auto suite = std::make_shared<SimpleTestFramework::TestSuite>(L## #suiteName, L## description); \
            SimpleTestFramework::TestRunner::GetInstance().AddTestSuite(suite); \
        } \
    }; \
    static suiteName##_TestSuite suiteName##_Instance;

#define ASSERT_TRUE(condition) SimpleTestFramework::TestAssert::IsTrue(condition, L## #condition)
#define ASSERT_FALSE(condition) SimpleTestFramework::TestAssert::IsFalse(condition, L## #condition)
#define ASSERT_EQUAL(expected, actual) SimpleTestFramework::TestAssert::AreEqual(expected, actual, L## #expected L" == " L## #actual)
#define ASSERT_NOT_EQUAL(expected, actual) SimpleTestFramework::TestAssert::AreNotEqual(expected, actual, L## #expected L" != " L## #actual)
#define ASSERT_SUCCEEDED(hr) SimpleTestFramework::TestAssert::Succeeded(hr, L## #hr)
#define ASSERT_FAILED(hr) SimpleTestFramework::TestAssert::Failed(hr, L## #hr)
#define ASSERT_NULL(ptr) SimpleTestFramework::TestAssert::IsNull(ptr, L## #ptr)
#define ASSERT_NOT_NULL(ptr) SimpleTestFramework::TestAssert::IsNotNull(ptr, L## #ptr)

#define REGISTER_TEST_CASE(testCase, suite) \
    suite->AddTestCase(std::make_shared<testCase>())

}
