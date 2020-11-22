#include "neko/Neko.h"
#include "neko/states/ChaseMouse.h"

namespace neko
{

Rectangle Neko::sprite()
{
    auto animation = _behavior->animation(*this);

    auto sprite = animation_play(animation, _tick);

    auto x = ((uint8_t)sprite >> 4) & 0xf;
    auto y = ((uint8_t)sprite >> 0) & 0xf;

    return {x * SIZE, y * SIZE, SIZE, SIZE};
}

Neko::Neko(Vec2f starting_position) :
    _position(starting_position),
    _sprites(Bitmap::load_from_or_placeholder("/Applications/neko/sprites.png")),
    _behavior(own<ChaseMouse>())
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
        auto vec = _position.vector_to(destination);
        _position = _position + (vec * SPEED);
    }

    did_update();
}

void Neko::behavior(OwnPtr<Behavior> behavior)
{
    _tick = 0;
    _behavior = behavior;
    did_update();
}

} // namespace neko
