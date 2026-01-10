#include "pch.h"
#include "TestBase.h"
#include "../Snap.Hutao.Remastered.Native/HutaoString.h"
#include "../Snap.Hutao.Remastered.Native/HutaoAString.h"
#include "../Snap.Hutao.Remastered.Native/Array.h"

class HutaoStringTests
{
private:
    TestResults& m_results;

public:
    HutaoStringTests(TestResults& results) : m_results(results) {}

    void RunAllTests()
    {
        TestConstructors();
        TestAssignment();
        TestConcatenation();
        TestComparison();
        TestSubstring();
        TestFindMethods();
        TestModificationMethods();
        TestConversionMethods();
        TestArrayIntegration();
        TestHutaoAString();
    }

private:
    void TestConstructors()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Constructors ===\n");
        
        // Default constructor
        {
            HutaoString str;
            bool passed = str.Length() == 0 && str.IsEmpty();
            TestAssert(passed, L"Default constructor failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Default constructor\n");
        }
        
        // Constructor from const wchar_t*
        {
            HutaoString str(L"Hello");
            bool passed = str.Length() == 5 && str == L"Hello";
            TestAssert(passed, L"Constructor from const wchar_t* failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Constructor from const wchar_t*\n");
        }
        
        // Constructor from another HutaoString
        {
            HutaoString str1(L"World");
            HutaoString str2(str1);
            bool passed = str2.Length() == 5 && str2 == L"World" && str1 == str2;
            TestAssert(passed, L"Copy constructor failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Copy constructor\n");
        }
        
        // Constructor with length
        {
            HutaoString str(L"Hello World", 5);
            bool passed = str.Length() == 5 && str == L"Hello";
            TestAssert(passed, L"Constructor with length failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Constructor with length\n");
        }
        
        timer.PrintElapsed(L"Constructors");
    }

    void TestAssignment()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Assignment ===\n");
        
        // Assignment from const wchar_t*
        {
            HutaoString str;
            str = L"Test";
            bool passed = str.Length() == 4 && str == L"Test";
            TestAssert(passed, L"Assignment from const wchar_t* failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Assignment from const wchar_t*\n");
        }
        
        // Assignment from another HutaoString
        {
            HutaoString str1(L"Source");
            HutaoString str2;
            str2 = str1;
            bool passed = str2.Length() == 6 && str2 == L"Source" && str1 == str2;
            TestAssert(passed, L"Assignment from HutaoString failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Assignment from HutaoString\n");
        }
        
        // Move assignment
        {
            HutaoString str1(L"MoveMe");
            HutaoString str2;
            str2 = std::move(str1);
            bool passed = str2.Length() == 6 && str2 == L"MoveMe" && str1.IsEmpty();
            TestAssert(passed, L"Move assignment failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Move assignment\n");
        }
        
        timer.PrintElapsed(L"Assignment");
    }

    void TestConcatenation()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Concatenation ===\n");
        
        // += operator with const wchar_t*
        {
            HutaoString str(L"Hello");
            str += L" World";
            bool passed = str.Length() == 11 && str == L"Hello World";
            TestAssert(passed, L"+= operator with const wchar_t* failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ += operator with const wchar_t*\n");
        }
        
        // += operator with HutaoString
        {
            HutaoString str1(L"Hello");
            HutaoString str2(L" World");
            str1 += str2;
            bool passed = str1.Length() == 11 && str1 == L"Hello World";
            TestAssert(passed, L"+= operator with HutaoString failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ += operator with HutaoString\n");
        }
        
        // + operator
        {
            HutaoString str1(L"Hello");
            HutaoString str2(L" World");
            HutaoString result = str1 + str2;
            bool passed = result.Length() == 11 && result == L"Hello World";
            TestAssert(passed, L"+ operator failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ + operator\n");
        }
        
        // Append method
        {
            HutaoString str(L"Hello");
            str.Append(L" World", 6);
            bool passed = str.Length() == 11 && str == L"Hello World";
            TestAssert(passed, L"Append method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Append method\n");
        }
        
        timer.PrintElapsed(L"Concatenation");
    }

    void TestComparison()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Comparison ===\n");
        
        // == operator
        {
            HutaoString str1(L"Hello");
            HutaoString str2(L"Hello");
            bool passed = str1 == str2;
            TestAssert(passed, L"== operator failed for equal strings");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ == operator (equal)\n");
        }
        
        // != operator
        {
            HutaoString str1(L"Hello");
            HutaoString str2(L"World");
            bool passed = str1 != str2;
            TestAssert(passed, L"!= operator failed for different strings");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ != operator (different)\n");
        }
        
        // < operator
        {
            HutaoString str1(L"Apple");
            HutaoString str2(L"Banana");
            bool passed = str1 < str2;
            TestAssert(passed, L"< operator failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ < operator\n");
        }
        
        // > operator
        {
            HutaoString str1(L"Banana");
            HutaoString str2(L"Apple");
            bool passed = str1 > str2;
            TestAssert(passed, L"> operator failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ > operator\n");
        }
        
        // CompareTo method
        {
            HutaoString str1(L"Hello");
            HutaoString str2(L"Hello");
            bool passed = str1.CompareTo(str2) == 0;
            TestAssert(passed, L"CompareTo method failed for equal strings");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ CompareTo method (equal)\n");
        }
        
        timer.PrintElapsed(L"Comparison");
    }

    void TestSubstring()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Substring ===\n");
        
        // Substring with start index
        {
            HutaoString str(L"Hello World");
            HutaoString sub = str.Substring(6, str.Length() - 6);
            bool passed = sub == L"World";
            TestAssert(passed, L"Substring with start index failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Substring with start index\n");
        }
        
        // Substring with start index and length
        {
            HutaoString str(L"Hello World");
            HutaoString sub = str.Substring(0, 5);
            bool passed = sub == L"Hello";
            TestAssert(passed, L"Substring with start index and length failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Substring with start index and length\n");
        }
        
        timer.PrintElapsed(L"Substring");
    }

    void TestFindMethods()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Find Methods ===\n");
        
        // Contains
        {
            HutaoString str(L"Hello World");
            bool passed = str.Contains(L"World") && !str.Contains(L"Universe");
            TestAssert(passed, L"Contains method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Contains method\n");
        }
        
        // IndexOf
        {
            HutaoString str(L"Hello World");
            bool passed = str.IndexOf(L'W') == 6 && str.IndexOf(L'z') == -1;
            TestAssert(passed, L"IndexOf method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ IndexOf method\n");
        }
        
        // LastIndexOf
        {
            HutaoString str(L"Hello World");
            bool passed = str.LastIndexOf(L'l') == 9;
            TestAssert(passed, L"LastIndexOf method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ LastIndexOf method\n");
        }
        
        // IndexOf with string
        {
            HutaoString str(L"Hello World");
            bool passed = str.IndexOf(L"World") == 6;
            TestAssert(passed, L"IndexOf with string failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ IndexOf with string\n");
        }
        
        timer.PrintElapsed(L"Find Methods");
    }

    void TestModificationMethods()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Modification Methods ===\n");
        
        // Replace
        {
            HutaoString str(L"Hello World");
            HutaoString replaced = str.Replace(L"World", L"Universe");
            bool passed = replaced == L"Hello Universe";
            TestAssert(passed, L"Replace method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Replace method\n");
        }
        
        // Trim
        {
            HutaoString str(L"  Hello World  ");
            HutaoString trimmed = str.Trim();
            bool passed = trimmed == L"Hello World";
            TestAssert(passed, L"Trim method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Trim method\n");
        }
        
        // TrimStart
        {
            HutaoString str(L"  Hello World");
            HutaoString trimmed = str.TrimStart();
            bool passed = trimmed == L"Hello World";
            TestAssert(passed, L"TrimStart method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ TrimStart method\n");
        }
        
        // TrimEnd
        {
            HutaoString str(L"Hello World  ");
            HutaoString trimmed = str.TrimEnd();
            bool passed = trimmed == L"Hello World";
            TestAssert(passed, L"TrimEnd method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ TrimEnd method\n");
        }
        
        // ToUpper
        {
            HutaoString str(L"Hello");
            HutaoString upper = str.ToUpper();
            bool passed = upper == L"HELLO";
            TestAssert(passed, L"ToUpper method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ ToUpper method\n");
        }
        
        // ToLower
        {
            HutaoString str(L"HELLO");
            HutaoString lower = str.ToLower();
            bool passed = lower == L"hello";
            TestAssert(passed, L"ToLower method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ ToLower method\n");
        }
        
        timer.PrintElapsed(L"Modification Methods");
    }

    void TestConversionMethods()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Conversion Methods ===\n");
        
        // Data() method
        {
            HutaoString str(L"Hello");
            const wchar_t* data = str.Data();
            bool passed = wcscmp(data, L"Hello") == 0;
            TestAssert(passed, L"Data() method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Data() method\n");
        }
        
        // operator LPCWSTR conversion
        {
            HutaoString str(L"Hello");
            LPCWSTR cstr = str;
            bool passed = wcscmp(cstr, L"Hello") == 0;
            TestAssert(passed, L"operator LPCWSTR conversion failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ operator LPCWSTR conversion\n");
        }
        
        // Format static method
        {
            HutaoString str = HutaoString::Format(L"Hello %s", L"World");
            bool passed = str == L"Hello World";
            TestAssert(passed, L"Format static method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Format static method\n");
        }
        
        timer.PrintElapsed(L"Conversion Methods");
    }

    void TestArrayIntegration()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing Array Integration ===\n");
        
        // Split method
        {
            HutaoString str(L"Hello,World,Test");
            hutao::Array<HutaoString> parts = str.Split(L',');
            bool passed = parts.Length() == 3 && parts[0] == L"Hello" && parts[1] == L"World" && parts[2] == L"Test";
            TestAssert(passed, L"Split method failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Split method\n");
        }
        
        // Array of HutaoString
        {
            hutao::Array<HutaoString> arr(3);
            arr[0] = L"First";
            arr[1] = L"Second";
            arr[2] = L"Third";
            
            bool passed = arr.Length() == 3 && arr[0] == L"First" && arr[1] == L"Second" && arr[2] == L"Third";
            TestAssert(passed, L"Array of HutaoString failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ Array of HutaoString\n");
        }
        
        timer.PrintElapsed(L"Array Integration");
    }

    void TestHutaoAString()
    {
        TestTimer timer;
        wprintf(L"\n=== Testing HutaoAString ===\n");
        
        // Basic HutaoAString tests
        {
            HutaoAString str("Hello");
            bool passed = str.Length() == 5 && str == "Hello";
            TestAssert(passed, L"HutaoAString basic test failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ HutaoAString basic test\n");
        }
        
        // HutaoAString concatenation
        {
            HutaoAString str1("Hello");
            HutaoAString str2(" World");
            str1 += str2;
            bool passed = str1 == "Hello World";
            TestAssert(passed, L"HutaoAString concatenation failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ HutaoAString concatenation\n");
        }
        
        // HutaoAString basic operations
        {
            HutaoAString astr("Hello");
            HutaoAString astr2(" World");
            astr += astr2;
            bool passed = astr == "Hello World";
            TestAssert(passed, L"HutaoAString concatenation failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ HutaoAString concatenation\n");
        }
        
        // HutaoAString comparison
        {
            HutaoAString astr1("Hello");
            HutaoAString astr2("Hello");
            bool passed = astr1 == astr2;
            TestAssert(passed, L"HutaoAString comparison failed");
            m_results.AddTest(passed);
            if (passed) wprintf(L"  ✓ HutaoAString comparison\n");
        }
        
        timer.PrintElapsed(L"HutaoAString");
    }
};

void RunHutaoStringTests()
{
    TestResults results;
    TestTimer totalTimer;
    
    wprintf(L"\n");
    wprintf(L"========================================\n");
    wprintf(L"Starting HutaoString Unit Tests\n");
    wprintf(L"========================================\n");
    
    HutaoStringTests tests(results);
    tests.RunAllTests();
    
    totalTimer.PrintElapsed(L"Total test time");
    results.PrintSummary();
    
    if (results.AllPassed())
    {
        wprintf(L"\n✅ All HutaoString tests passed!\n");
    }
    else
    {
        wprintf(L"\n❌ Some HutaoString tests failed!\n");
    }
}
