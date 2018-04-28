#include "catch.hpp"
#include <iostream>

TEST_CASE("Sections execute preceding lines of code.")
{
    int num = 0;

    SECTION("Increment once")
    {
        ++num;
    }
    SECTION("Increment again")
    {
        num += 2;
    }
    SECTION("Increment yet again")
    {
        num += 3;
        REQUIRE(num == 3);
    }
}
