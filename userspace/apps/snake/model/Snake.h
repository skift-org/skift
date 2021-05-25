#pragma once

#include <libmath/Vec2.h>
#include <libutils/Vector.h>

#include "snake/model/Fruit.h"

namespace Snake
{

enum struct Facing
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

using Head = Math::Vec2i;
using Tail = Vector<Math::Vec2i>;

struct Snake
{
private:
    Facing _facing;
    Tail _tail;
    Head _head;
    int _length;

public:
    const Facing &facing() const { return _facing; }
    const Tail &tail() const { return _tail; }
    const Head &head() const { return _head; }
    int length() const { return _length; }

    Snake();

    Snake(Head start);

    void face_to(Facing new_facing);

    void move();

    bool colide_with(Math::Vec2i p);

    bool colide_with_head(Math::Vec2i p);

    bool colide_with_body(Math::Vec2i p);

    void eat();
};

} // namespace Snake
