#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>

#include "neko/Animation.h"

namespace neko
{

static Animation direction_to_animation[3][3] = {
    {Animation::MOVE_UP_LEFT, Animation::MOVE_UP, Animation::MOVE_UP_RIGHT},
    {Animation::MOVE_LEFT, Animation::STOP, Animation::MOVE_RIGHT},
    {Animation::MOVE_DOWN_LEFT, Animation::MOVE_DOWN, Animation::MOVE_DOWN_RIGHT},
};

Animation animation_from_vector(Vec2f vec)
{
    auto normalized = vec.normalized();

    int x = normalized.x() * 2;
    int y = normalized.y() * 2;

    return direction_to_animation[clamp(y + 1, 0, 2)][clamp(x + 1, 0, 2)];
}

Sprite animations[][2] = {
    /* STOP             */ {Sprite::MATI1, Sprite::MATI1},
    /* JARE             */ {Sprite::JARE, Sprite::MATI1},
    /* KAKI             */ {Sprite::KAKI1, Sprite::KAKI2},
    /* AKUBI            */ {Sprite::MATI2, Sprite::MATI2},
    /* SLEEP            */ {Sprite::SLEEP1, Sprite::SLEEP2},
    /* AWAKE            */ {Sprite::AWAKE, Sprite::AWAKE},

    /* MOVE_UP          */ {Sprite::UP1, Sprite::UP2},
    /* MOVE_DOWN        */ {Sprite::DOWN1, Sprite::DOWN2},
    /* MOVE_LEFT        */ {Sprite::LEFT1, Sprite::LEFT2},
    /* MOVE_RIGHT       */ {Sprite::RIGHT1, Sprite::RIGHT2},

    /* MOVE_UP_LEFT     */ {Sprite::UPLEFT1, Sprite::UPLEFT2},
    /* MOVE_UP_RIGHT    */ {Sprite::UPRIGHT1, Sprite::UPRIGHT2},
    /* MOVE_DOWN_LEFT   */ {Sprite::DWLEFT1, Sprite::DWLEFT2},
    /* MOVE_DOWN_RIGHT  */ {Sprite::DWRIGHT1, Sprite::DWRIGHT2},

    /* TOGI_UP          */ {Sprite::UTOGI1, Sprite::UTOGI2},
    /* TOGI_DOWN        */ {Sprite::DTOGI1, Sprite::DTOGI2},
    /* TOGI_LEFT        */ {Sprite::LTOGI1, Sprite::LTOGI2},
    /* TOGI_RIGHT       */ {Sprite::RTOGI1, Sprite::RTOGI2},
};

Sprite animation_play(Animation animation, int tick)
{
    auto animation_tick = (tick) % 2;

    if (animation == Animation::SLEEP)
    {
        animation_tick = (tick / 3) % 2;
    }

    return animations[(int)animation][animation_tick];
}

} // namespace neko
