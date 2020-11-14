#pragma once

#include <libsystem/math/Vec2.h>

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

Animation vector_to_move_animation(Vec2f);

} // namespace neko
