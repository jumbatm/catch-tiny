# Catch Tiny - A light, header-only testing framework
based on Catch2.

If you only need `TEST_CASE`, `SECTION` and `REQUIRE` macros, simply replace `catch.hpp`.

# Usage
Testing revolves around the macros described above. 
A test is declared with 
```cpp
TEST_CASE(const char *name)
{
    // ...
}
```
, where you can name your test. This name is thrown back at you if any assertions you make fail, so you know where to look. Inside these test cases, you can use

```cpp
REQUIRE(bool b);
```
to make assertions of your code. If these fail, you'll see output like:
```
In test case: "Internal pointer is set to null after class is destructed."
	Assertion failed: REQUIRE(ptr == nullptr) at sections.cpp:27
```
Just like with Catch2, make sure you define `CATCH_CONFIG_MAIN` before including `catch.hpp` in one file, like:
```cpp
#define CATCH_CONFIG_MAIN // Define this in ONE FILE ONLY.
#include "catch.hpp"
```
Catch Tiny will provide a main function which runs all the tests compiled with it.

In its most basic form, a test might look something like this:

#### main.cpp

```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
```

#### and_test.cpp

```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template <bool B1, bool B2>
struct And
{
    static constexpr bool value = false;
};

template <>
struct And<true, true>
{
    static constexpr bool value = true;
};

TEST_CASE("And struct behaves like logical AND")
{
    REQUIRE(And<true, true>::value);
    REQUIRE(And<false, true>::value == false);
    REQUIRE(And<true, false>::value == false);
}

```
Compiling and running this, we get:
```
1 of 1 test cases passed.
```
Fantastic.

### Sections (still in progress)
If you have a number of individual tests you want to perform in a specific section, you can use 
```cpp
// ...
SECTION(const char *name)
{
    // ...
}
// ...
```
within a `TEST_CASE`. Anything outside a `SECTION` in the test case gets executed before each section, so this is useful if your tests all have something they need initialised in the same way, like in the below example:
```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
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

```
This also outputs
```
1 of 1 test cases passed.
```

# Also see:
- [Catch2](https://github.com/catchorg/Catch2), the original testing framework this is based off. Has a ton of features.
- [Catch Mini](https://github.com/GValiente/catch-mini), another minimal subset of Catch2.
