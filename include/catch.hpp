#ifndef __CATCH_TINY_INCLUDED_H__
#define __CATCH_TINY_INCLUDED_H__

#include <iostream>
#include <unordered_map>
#include <string>

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
    TestCase TEST_NAME(CASE_NAME, FUNC_NAME); \
    static void FUNC_NAME(TestCase *this_)

// Public interface.
#define TEST_CASE(x) CATCH_TINY_GENERATE( PP_CONCAT(f, __LINE__), PP_CONCAT(t, __LINE__), x)
#define SECTION(...) if (CATCH_INTERNAL(idx) == 0) this_->sections++; \
                                              if (CATCH_INTERNAL(idx) == __COUNTER__)
#define REQUIRE(...) Assertion::Assert(#__VA_ARGS__, __FILE__, __LINE__, __VA_ARGS__)

struct TestCase
{
    static std::unordered_map<std::string, TestCase> allTestCases;
    const char *name;
    void (*function)(TestCase*);
    size_t sections = 1;

    TestCase(const char *name, void (*function)(TestCase*));
};

struct Assertion
{
    size_t line;
    const char *expression;
    const char *file;

    static void Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion);
};

extern size_t CATCH_INTERNAL(x);

#ifdef CATCH_CONFIG_MAIN

size_t CATCH_INTERNAL(idx) = 0;

std::unordered_map<std::string, TestCase> TestCase::allTestCases;
void Assertion::Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion)
{
    if (!assertion)
    {
        throw Assertion{lineNumber, exp, fileName};
    }
}
TestCase::TestCase(const char *name, void (*function)(TestCase*)) : name(name), function(function)
{
    allTestCases.insert({std::string(name), *this});
}

int main()
{
    for (auto& pair : TestCase::allTestCases)
    {    
        CATCH_INTERNAL(idx) = 0;

        auto& testCase = pair.second;
        try
        {
            do
            {
                testCase.function(&testCase);
                ++CATCH_INTERNAL(idx);
            } while (--testCase.sections);
        }
        catch (Assertion& a)
        {
            std::cout << "In test case: " << testCase.name << "\n" <<
                            "\tAssertion failed: " <<  "REQUIRE(" << a.expression << ") at " << a.file << ":" << a.line << "\n";
            break;
        }
    }

    return 0;
}
#endif
#endif
