#ifndef _cATCH_TINY_INCLUDED_H_
#define _cATCH_TINY_INCLUDED_H_

#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Configuration.
//#define CATCH_EXIT_ON_FAIL

// Preprocessor utilities.
#define PP_CONCAT_(x, y) x##y
#define PP_CONCAT(x, y) \
    PP_CONCAT_(x, y)  // Indirection, as PP won't recursively stringise.

// Generate a name for internal variables to reduce likelihood of collisions.
#define CATCH_INTERNAL(x) catch_tiny_internal_##x

// Force execution outside of main.
#define CATCH_EXEC(x)                                  \
    static bool PP_CONCAT(_cATCH_TINY_EXEC_, __LINE__) \
    {                                                  \
        (x)                                            \
    }

#if __cplusplus < 201703L
// Pre-C++17, we use implementation-defined attributes.
#    if defined(__GNUC__) || defined(__llvm__)
// gcc or clang.
#        define CATCH_POTENTIALLY_UNUSED __attribute__((unused))
#    else
// Otherwise, don't bother. Optimally this
#        define CATCH_POTENTIALLY_UNUSED
#    endif
#else
// Post C++17, maybe_unused is standardised.
#    define CATCH_POTENTIALLY_UNUSED [[maybe_unused]]
#endif

// Create a new global test case object. Called by TEST_CASE.
#define CATCH_TINY_GENERATE(FUNC_NAME, TEST_NAME, CASE_NAME)   \
    static void FUNC_NAME(TestCase *);                         \
    static TestCase TEST_NAME(__FILE__, CASE_NAME, FUNC_NAME); \
    static void FUNC_NAME(CATCH_POTENTIALLY_UNUSED TestCase *this_)

// Public interface.
#define TEST_CASE(x)                                                  \
    CATCH_TINY_GENERATE(PP_CONCAT(_cATCH_TINY_TEST_CASE_, __LINE__),  \
                        PP_CONCAT(_cATCH_TINY_TEST_CASE_b, __LINE__), \
                        x)
#define SECTION(x)                \
    if (CATCH_INTERNAL(idx) == 0) \
        this_->sections++;        \
    if (CATCH_INTERNAL(idx) == (this_->section = x, __COUNTER__))
#define REQUIRE(...) \
    Assertion::Assert(#__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)

#ifdef CATCH_EXIT_ON_FAIL
#    define CATCH_FAILED_TEST_BREAK break
#else
#    define CATCH_FAILED_TEST_BREAK
#endif

struct TestCase
{
    static size_t count;

    const char *const name;
    const char *section = "<None>";

    void (*function)(TestCase *);
    size_t sections = 0;

    TestCase(const char *filename,
             const char *name,
             void (*function)(TestCase *));

    static std::unordered_map<std::string, std::list<TestCase *>>
        &getAllTestCases();
    static std::unordered_set<std::string> &getTestCaseNames();
};

struct Assertion
{
    size_t line;
    const char *expression;
    const char *file;

    static void Assert(const char *exp,
                       const char *fileName,
                       size_t lineNumber,
                       bool assertion);
};

extern size_t CATCH_INTERNAL(idx);
extern const char *CATCH_INTERNAL(duplicateTestCaseName);

#ifdef CATCH_CONFIG_MAIN

size_t CATCH_INTERNAL(idx)                        = 0;
const char *CATCH_INTERNAL(duplicateTestCaseName) = nullptr;

std::unordered_map<std::string, std::list<TestCase *>>
    &TestCase::getAllTestCases()
{
    static std::unordered_map<std::string, std::list<TestCase *>>
        TestCase_allTestCases;
    return TestCase_allTestCases;
}
std::unordered_set<std::string> &TestCase::getTestCaseNames()
{
    static std::unordered_set<std::string> TestCase_testCaseNames;
    return TestCase_testCaseNames;
}

size_t TestCase::count = 0;

void Assertion::Assert(const char *exp,
                       const char *fileName,
                       size_t lineNumber,
                       bool assertion)
{
    if (!assertion)
    {
        throw Assertion{ lineNumber, exp, fileName };
    }
}
TestCase::TestCase(const char *filename,
                   const char *name,
                   void (*function)(TestCase *))
  : name(name), function(function)
{
    TestCase::count++;
    TestCase::getAllTestCases()[std::string(filename)].push_back(this);
    if (!TestCase::getTestCaseNames().insert(std::string(name)).second)
    {
        CATCH_INTERNAL(duplicateTestCaseName) = name;
    }
}

int main()
{
    bool success = true;
    if (CATCH_INTERNAL(duplicateTestCaseName))
    {
        printf("Duplicate test case name: \"%s\".\n",
               CATCH_INTERNAL(duplicateTestCaseName));
        return -1;
    }
    size_t testCasesPassed = 0;

    for (auto &filenameToTestCaseList : TestCase::getAllTestCases())
    {
        CATCH_INTERNAL(idx) = 0;

        for (auto &testCase : filenameToTestCaseList.second)
        {
            try
            {
                do
                {
                    testCase->function(testCase);

                    if (CATCH_INTERNAL(idx) == 0 && testCase->sections > 0)
                    {
                        --testCase->sections;
                    }

                    ++CATCH_INTERNAL(idx);

                } while (testCase->sections--);
            }
            catch (Assertion &a)
            {
                std::cout << "In test case: \"" << testCase->name << "\"\n"
                          << "... In section: \"" << testCase->section << "\"\n"
                          << "\tAssertion failed: "
                          << "REQUIRE(" << a.expression << ") at " << a.file
                          << ":" << a.line << "\n";
                success = false;
                CATCH_FAILED_TEST_BREAK;
            }
            catch (std::exception &e)
            {
                std::cout << "In test case: \"" << testCase->name
                          << "\"\n"
                             "... In section: \""
                          << testCase->section << "\"\n"
                          << "\tAn exception was thrown. what(): " << e.what()
                          << "\n";
                success = false;
                CATCH_FAILED_TEST_BREAK;
            }
            catch (...)
            {
                std::cout << "In test case: \"" << testCase->name
                          << "\"\n"
                             "... In section: \""
                          << testCase->section << "\"\n"
                          << "\tAn unrecognised object was thrown. Aborting."
                          << "\n";
                success = false;
                CATCH_FAILED_TEST_BREAK;
            }
            ++testCasesPassed;
        }
    }
#    ifdef CATCH_EXIT_ON_FAIL
    if (success)
        printf("All %zd tests passed.\n", TestCase::count);
#    else
    printf("%zd of %zd test cases passed.\n", testCasesPassed, TestCase::count);
#    endif
    return (success ? 0 : -1);
}
#endif
#endif
