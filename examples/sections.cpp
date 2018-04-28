#include "catch.hpp"
#include <iostream>

TEST_CASE("Sections execute preceding lines of code.")
{
    int num = 0;

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
