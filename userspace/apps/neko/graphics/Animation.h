#pragma once

#include <libmath/Vec2.h>

#include "neko/graphics/Sprites.h"

namespace neko
{

struct Animation
{
private:
    Sprite _frames[2];
    int _speed = 1;

public:
    constexpr Animation(Sprite frame) : _frames{frame, frame}
    {
    }

    constexpr Animation(Sprite first_frame, Sprite second_frame)
        : _frames{first_frame, second_frame}
    {
    }

    constexpr Animation(Sprite first_frame, Sprite second_frame, int speed)
        : _frames{first_frame, second_frame}, _speed(speed)
    {
    }

    Sprite play(int tick)
    {
        auto animation_tick = (tick / _speed) % 2;

        return _frames[animation_tick];
    }
};

} // namespace neko
