#ifndef __CATCH_TINY_INCLUDED_H__
#define __CATCH_TINY_INCLUDED_H__

#include <iostream>
#include <unordered_map>
#include <string>

#define PP_CONCAT_(x, y) x ## y
#define PP_CONCAT(x, y) PP_CONCAT_(x, y) // Indirection, as PP won't recursively stringise.

#define CATCH_TINY_GENERATE(FUNC_NAME, TEST_NAME, CASE_NAME) \
    static void FUNC_NAME(); \
    TestCase TEST_NAME(CASE_NAME, FUNC_NAME); \
    static void FUNC_NAME()

#define REQUIRE(EXPRESSION) Assertion::Assert(#EXPRESSION, __FILE__, __LINE__, EXPRESSION)

#define TEST_CASE(x) CATCH_TINY_GENERATE( PP_CONCAT(f, __LINE__), PP_CONCAT(t, __LINE__), x)

struct TestCase
{
    static std::unordered_map<std::string, TestCase> allTestCases;
    const char *name;
    void (*function)();

    TestCase(const char *name, void (*function)());
};

struct Assertion
{
    size_t line;
    const char *expression;
    const char *file;

    static void Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion);
};

#ifdef CATCH_CONFIG_MAIN

std::unordered_map<std::string, TestCase> TestCase::allTestCases;
void Assertion::Assert(const char *exp, const char *fileName, size_t lineNumber, bool assertion)
{
    if (!assertion)
    {
        throw Assertion{lineNumber, exp, fileName};
    }
}
TestCase::TestCase(const char *name, void (*function)()) : name(name), function(function)
{
    allTestCases.insert({std::string(name), *this});
}


int main()
{
    for (auto& i : TestCase::allTestCases)
    {
        try
        {
            i.second.function();
        }
        catch (Assertion& a)
        {
            std::cout << "In test case " << i.second.name << "\n" <<
                            "\tAssertion failed: " <<  "REQUIRE(" << a.expression << ") at " << a.file << ":" << a.line << std::endl;
        }
        break;
    }

    return 0;
}
#endif
#endif
