#include <libutils/String.h>

#include "tests/Driver.h"

TEST(string_with_content)
{
    String hello_world = "Hello, world!";

    assert_equal(hello_world, "Hello, world!");
    assert_not_equal(hello_world, "Something else blablabla!");
    assert_false(hello_world.null_or_empty());
}

TEST(empty_strings)
{
    String empty = "";

    assert_equal(empty, "");
    assert_not_equal(empty, "Something else blablabla!");
    assert_true(empty.null_or_empty());
}

TEST(string_move_operator)
{
    String first = "first";
    String second = "second";

    first = move(second);

    assert_equal(first, "second");
    assert_equal(second, "first");
}
