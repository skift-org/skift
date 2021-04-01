#include <libutils/String.h>

#include "tests/Driver.h"

TEST(string_with_content)
{
    String hello_world = "Hello, world!";

    Assert::equal(hello_world, "Hello, world!");
    Assert::not_equal(hello_world, "Something else blablabla!");
    Assert::is_false(hello_world.null_or_empty());
}

TEST(empty_strings)
{
    String empty = "";

    Assert::equal(empty, "");
    Assert::not_equal(empty, "Something else blablabla!");
    Assert::is_true(empty.null_or_empty());
}

TEST(string_move_operator)
{
    String first = "first";
    String second = "second";

    first = move(second);

    Assert::equal(first, "second");
    Assert::equal(second, "first");
}
