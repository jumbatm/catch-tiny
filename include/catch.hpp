#ifndef __CATCH_TINY_INCLUDED_H__
#define __CATCH_TINY_INCLUDED_H__

#include <iostream>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <stdexcept>

// Preprocessor utilities.
#define PP_CONCAT_(x, y) x ## y
#define PP_CONCAT(x, y) PP_CONCAT_(x, y) // Indirection, as PP won't recursively stringise.

// Generate a name for internal variables to reduce likelihood of collisions.
#define CATCH_INTERNAL(x) catch_tiny_internal__ ## x

// Force execution outside of main.
#define CATCH_EXEC(x) static bool PP_CONCAT( b, __LINE__){(x)}

// Create a new global test case object. Called by TEST_CASE.
#define CATCH_TINY_GENERATE(FUNC_NAME, TEST_NAME, CASE_NAME) \
    static void FUNC_NAME(TestCase*); \
    TestCase TEST_NAME(__FILE__, CASE_NAME, FUNC_NAME); \
    static void FUNC_NAME(TestCase *this_)

// Public interface.
#define TEST_CASE(x) CATCH_TINY_GENERATE( PP_CONCAT(f, __LINE__), PP_CONCAT(t, __LINE__), x)
#define SECTION(x) if (CATCH_INTERNAL(idx) == 0) this_->sections++; \
                                              if (CATCH_INTERNAL(idx) == (this_->section = x, __COUNTER__))
#define REQUIRE(...) Assertion::Assert(#__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)

struct TestCase
{
    static std::unordered_map<std::string, std::list<TestCase> > allTestCases;
    static std::unordered_set<std::string> testCaseNames;
    static size_t count;

    const char *const name;
    const char *section;

    void (*function)(TestCase*);
    size_t sections = 0;

    TestCase(const char *filename, const char *name, void (*function)(TestCase*));
};

struct Assertion
{
    size_t line;
    const char *expression;
    const char *file;

    static void Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion);
};

extern size_t CATCH_INTERNAL(idx);
extern bool CATCH_INTERNAL(duplicateFlag);

#ifdef CATCH_CONFIG_MAIN

size_t CATCH_INTERNAL(idx) = 0;
bool CATCH_INTERNAL(duplicateFlag) = false;

std::unordered_set<std::string> TestCase::testCaseNames;
std::unordered_map<std::string, std::list<TestCase> > TestCase::allTestCases;
size_t TestCase::count = 0;

void Assertion::Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion)
{
    if (!assertion)
    {
        throw Assertion{lineNumber, exp, fileName};
    }
}
TestCase::TestCase(const char *filename, const char *name, void (*function)(TestCase*)) : name(name), function(function)
{
    TestCase::count++;
    allTestCases[std::string(filename)].push_back(*this);
    if (!testCaseNames.insert(std::string(name)).second)
    {
        CATCH_INTERNAL(duplicateFlag) = true;
    }
}

int main()
{
    bool success = true;
    if (CATCH_INTERNAL(duplicateFlag))
    {
        printf("Duplicate test case flags.\n");
        return -1;
    }
    size_t testCasesPassed = 0;

    for (auto& filenameToTestCaseList : TestCase::allTestCases)
    {    
        CATCH_INTERNAL(idx) = 0;

        for (auto& testCase : filenameToTestCaseList.second)
        {
            try
            {
                do
                {
                    testCase.function(&testCase);

                    if (CATCH_INTERNAL(idx) == 0 && testCase.sections > 0) 
                    {
                        --testCase.sections;
                    }

                    ++CATCH_INTERNAL(idx);

                } while (testCase.sections--);
            }
            catch (Assertion& a)
            {
                std::cout << "In test case: \"" << testCase.name << "\"\n" <<
                    "... In section: \"" << testCase.section << "\"\n" <<
                        "\tAssertion failed: " <<  "REQUIRE(" << a.expression << ") at " << a.file << ":" << a.line << "\n";
                success = false;
                break;
            }
            catch (std::exception& e)
            {
                std::cout << "In test case: \"" << testCase.name << "\"\n"
                    "... In section: \"" << testCase.section << "\"\n" <<
                        "\tAn exception was thrown. what(): " << e.what() << "\n";
                success = false;
                break;
            }
            catch (...)
            {
                std::cout << "In test case: \"" << testCase.name << "\"\n" 
                    "... In section: \"" << testCase.section << "\"\n" <<
                        "\tAn unrecognised object was thrown. Aborting." << "\n";
                success = false;
                break;
            }
            ++testCasesPassed;
        }

        printf("%zd of %zd test cases passed.\n", testCasesPassed,
                TestCase::count);

    return (success ? 0 : -1);
    }
}
#endif
#endif
