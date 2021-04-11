#include <libmath/Vec2.h>

#include "tests/Driver.h"

TEST(math_vec2_add)
{
    Math::Vec2f a{2.0f, 1.0f};
    Math::Vec2f b{3.0f, 4.0f};

    auto c = a + b;

    Assert::equal(c.x(), 5.0f);
    Assert::equal(c.y(), 5.0f);
}

TEST(math_vec2_sub)
{
    Math::Vec2f a{2.0f, 1.0f};
    Math::Vec2f b{3.0f, 4.0f};

    auto c = a - b;

    Assert::equal(c.x(), -1.0f);
    Assert::equal(c.y(), -3.0f);
}

TEST(math_vec2_dot)
{
    Math::Vec2f a{1.0f, 0.0f};
    Math::Vec2f b{-1.0f, 1.0f};

    auto c = a.dot(b);

    Assert::equal(c, -1.0f);
}