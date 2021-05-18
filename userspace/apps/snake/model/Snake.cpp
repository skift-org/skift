#include "snake/model/Snake.h"

namespace Snake
{

Math::Vec2i facing_to_vec(Facing facing)
{
    switch (facing)
    {
    case Facing::UP:
        return {0, -1};
    case Facing::DOWN:
        return {0, 1};

    case Facing::LEFT:
        return {-1, 0};

    case Facing::RIGHT:
        return {1, 0};
    }

    ASSERT_NOT_REACHED();
}

Snake::Snake()
{
}

Snake::Snake(Head head)
{
    _head = head;
    _facing = Facing::LEFT;

    for (int i = 0; i < 4; i++)
    {
        _tail.push_back(head + Math::Vec2i{1 + i, 0});
    }
}

void Snake::face_to(Facing new_facing)
{
    if (_facing == Facing::RIGHT && new_facing == Facing::LEFT)
    {
        return;
    }

    if (_facing == Facing::LEFT && new_facing == Facing::RIGHT)
    {
        return;
    }

    if (_facing == Facing::UP && new_facing == Facing::DOWN)
    {
        return;
    }

    if (_facing == Facing::DOWN && new_facing == Facing::UP)
    {
        return;
    }

    _facing = new_facing;
}

void Snake::move()
{
    _tail.push(_head);

    if (_tail.count() > (size_t)_length)
    {
        _tail.pop_back();
    }

    _head += facing_to_vec(_facing);
}

bool Snake::colide_with(Math::Vec2i p)
{
    return colide_with_head(p) || colide_with_body(p);
}

bool Snake::colide_with_head(Math::Vec2i p)
{
    return _head == p;
}

bool Snake::colide_with_body(Math::Vec2i p)
{
    for (auto &t : _tail)
    {
        if (t == p)
        {
            return true;
        }
    }

    return false;
}

void Snake::eat()
{
    _length += 1;
}

} // namespace Snake
