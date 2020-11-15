#pragma once

#include <libsystem/math/Vec2.h>

#include "neko/Sprites.h"

namespace neko
{

enum class Animation : int
{
    STOP,
    JARE,
    KAKI,
    AKUBI,
    SLEEP,
    AWAKE,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP_LEFT,
    MOVE_UP_RIGHT,
    MOVE_DOWN_LEFT,
    MOVE_DOWN_RIGHT,
    TOGI_UP,
    TOGI_DOWN,
    TOGI_LEFT,
    TOGI_RIGHT,
};

Animation animation_from_vector(Vec2f vec);

Sprite animation_play(Animation anim, int tick);

} // namespace neko
