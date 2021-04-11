#pragma once

#include <assert.h>

#include <libmath/Vec2.h>
#include <libutils/Vector.h>

namespace Snake
{

enum Facing
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

Math::Vec2i facing_to_vec(Facing facing)
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
    int score;
    Facing facing;
    Vector<Math::Vec2i> tail;
    Math::Vec2i head;

    Snake() {}

    Snake(Math::Vec2i start)
    {
        head = start;
        facing = Facing::LEFT;

        score = 4;

        for (int i = 0; i < 4; i++)
        {
            tail.push_back(start + Math::Vec2i{1 + i, 0});
        }
    }

    void face_to(Facing new_facing)
    {
        if (facing == RIGHT && new_facing == LEFT)
        {
            return;
        }

        if (facing == LEFT && new_facing == RIGHT)
        {
            return;
        }

        if (facing == UP && new_facing == DOWN)
        {
            return;
        }

        if (facing == DOWN && new_facing == UP)
        {
            return;
        }

        facing = new_facing;
    }

    void move()
    {
        tail.push(head);

        if (tail.count() > (size_t)score)
        {
            tail.pop_back();
        }

        head += facing_to_vec(facing);
    }

    bool colide_with(Math::Vec2i p)
    {
        return colide_with_head(p) || colide_with_body(p);
    }

    bool colide_with_head(Math::Vec2i p)
    {
        return head == p;
    }

    bool colide_with_body(Math::Vec2i p)
    {
        for (auto &t : tail)
        {
            if (t == p)
            {
                return true;
            }
        }

        return false;
    }
};

} // namespace Snake
