#include <libmath/Vec2.h>

#include "tests/Driver.h"

TEST(vec2_add)
{
    Vec2f a{2.0f, 1.0f};
    Vec2f b{3.0f, 4.0f};

    auto c = a + b;

    Assert::equal(c.x(), 5.0f);
    Assert::equal(c.y(), 5.0f);
}

TEST(vec2_sub)
{
    Vec2f a{2.0f, 1.0f};
    Vec2f b{3.0f, 4.0f};

    auto c = a - b;

    Assert::equal(c.x(), -1.0f);
    Assert::equal(c.y(), -3.0f);
}

TEST(vec2_dot)
{
    Vec2f a{1.0f, 0.0f};
    Vec2f b{-1.0f, 1.0f};

    auto c = a.dot(b);

    Assert::equal(c, -1.0f);
}