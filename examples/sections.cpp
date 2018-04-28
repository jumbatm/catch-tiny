#include "catch.hpp"

//#define MSG(x) std::cout << x << "\n";

#define MSG(x)

#include <iostream>

TEST_CASE("Sections execute preceding lines of code.")
{
    MSG("Entering test case");
    size_t idx = CATCH_INTERNAL(idx);
    size_t num = 0;

    SECTION("Increment once")
    {
        MSG("Entering section 1");
        ++num;
    }
    SECTION("Increment again")
    {
        MSG("Entering section 2");
        num += 2;
    }
    SECTION("Increment yet again")
    {
        MSG("Entering section 3");
        num += 3;
        REQUIRE(num == 3);
    }
}
