#pragma once

#include <libutils/Vec2.h>

namespace Snake
{

enum Facing
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

Vec2i facing_to_vec(Facing facing)
{
    switch (facing)
    {
    case UP:
        return {0, -1};
    case DOWN:
        return {0, 1};

    case LEFT:
        return {-1, 0};

    case RIGHT:
        return {1, 0};
    }

    ASSERT_NOT_REACHED();
}

struct Snake
{
    Facing facing;
    Vector<Vec2i> tail;
    Vec2i head;

    void move()
    {
        tail.push(head);
        tail.pop_back();
        head += facing_to_vec(facing);
    }
};

} // namespace Snake
