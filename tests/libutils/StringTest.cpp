#include <libutils/String.h>

#include "tests/Driver.h"

TEST(string_with_content)
{
    String hello_world = "Hello, world!";

    assert(hello_world == "Hello, world!");
    assert(hello_world != "Something else blablabla!");
    assert(!hello_world.null_or_empty());
}

TEST(empty_strings)
{
    String empty = "";

    assert(empty == "");
    assert(empty != "Something else blablabla!");
    assert(empty.null_or_empty());
}

TEST(string_move_operator)
{
    String first = "first";
    String second = "second";

    first = move(second);

    assert(first == "second");
    assert(second == "first");
}
