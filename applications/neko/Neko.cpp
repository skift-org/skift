#include "neko/Neko.h"
#include "neko/ChaseMouse.h"
#include "neko/Wandering.h"

namespace neko
{

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

Rectangle Neko::sprite()
{
    auto animation = _behavior->animation(*this);

    auto animation_tick = (_tick) % 2;

    if (animation == Animation::SLEEP)
    {
        animation_tick = (_tick / 3) % 2;
    }

    auto sprite = animations[(int)animation][animation_tick];

    auto x = ((uint8_t)sprite >> 4) & 0xf;
    auto y = ((uint8_t)sprite >> 0) & 0xf;

    return {x * SIZE, y * SIZE, SIZE, SIZE};
}

Neko::Neko(Vec2f starting_position)
    : _position(starting_position),
      _sprites(Bitmap::load_from_or_placeholder("/Applications/neko/sprites.png")),
      _behavior(own<ChaseMouse>())
{
}

Neko::~Neko()
{
}

void Neko::update()
{
    _behavior->update(*this);
    _tick++;
}

void Neko::paint(Painter &painter)
{
    painter.blit_bitmap(*_sprites, sprite(), {SIZE, SIZE});
}

void Neko::move_to(Vec2f destination)
{
    if (_position.distance_to(destination) < SPEED)
    {
        _position = destination;
    }
    else
    {
        auto vec  = _position.vector_to(destination);
        _position = _position + (vec * SPEED);
    }

    did_update();
}

void Neko::behavior(OwnPtr<Behavior> behavior)
{
    _tick     = 0;
    _behavior = behavior;
    did_update();
}

} // namespace neko
