#include <libio/Path.h>

#include "tests/Driver.h"

TEST(absolute_path_are_absolute)
{
    auto p = IO::Path::parse("/home/user/local");

    Assert::is_true(p.absolute());
}

TEST(absolute_path_are_no_relative)
{
    auto p = IO::Path::parse("/home/user/local");

    Assert::is_false(p.relative());
}

TEST(relative_path_are_not_absolute)
{
    auto p = IO::Path::parse("user/local");

    Assert::is_false(p.absolute());
}

TEST(relative_path_are_relative)
{
    auto p = IO::Path::parse("user/local");

    Assert::is_true(p.relative());
}

TEST(same_path_are_equals)
{
    auto p1 = IO::Path::parse("/home/user/local");
    auto p2 = IO::Path::parse("/home/user/local");

    Assert::equal(p1, p2);
}

TEST(different_path_are_not_equals)
{
    auto p1 = IO::Path::parse("/home/user/local/documents");
    auto p2 = IO::Path::parse("/home/user/local");

    Assert::not_equal(p1, p2);
}

TEST(a_path_and_its_string_representation_are_equals)
{
    auto p = IO::Path::parse("/home/user/local");
    Assert::equal(p.string(), "/home/user/local");
}

TEST(path_is_equal_to_its_normalized_counter_part)
{
    auto p = IO::Path::parse("/home/../home/././././user/local");
    p = p.normalized();

    Assert::equal(p.string(), "/home/user/local");
}

TEST(joined_path_are_equals)
{
    auto p1 = IO::Path::parse("/home/");
    auto p2 = IO::Path::join(p1, "user/local");

    Assert::equal(p2.string(), "/home/user/local");
}
