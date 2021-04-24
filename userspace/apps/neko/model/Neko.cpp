#include <libio/Streams.h>

#include "neko/model/Neko.h"
#include "neko/states/ChaseMouse.h"

namespace neko
{

Math::Recti Neko::sprite()
{
    auto animation = _behavior->animation(*this);

    auto sprite = animation.play(_tick);

    auto x = ((uint8_t)sprite >> 4) & 0xf;
    auto y = ((uint8_t)sprite >> 0) & 0xf;

    return {x * SIZE, y * SIZE, SIZE, SIZE};
}

Neko::Neko(Math::Vec2f starting_position)
    : _position(starting_position),
      _sprites(Graphic::Bitmap::load_from_or_placeholder("/Applications/neko/sprites.png")),
      _behavior(own<ChaseMouse>())
{
}

void Neko::update()
{
    _behavior->update(*this);

    if (_next_behavior != nullptr)
    {
        _behavior = _next_behavior;
        _next_behavior = nullptr;
        _tick = 0;
        did_update();
    }
    else
    {
        _tick++;
    }
}

void Neko::paint(Graphic::Painter &painter)
{
    painter.blit(*_sprites, sprite(), {SIZE, SIZE});
}

void Neko::move_to(Math::Vec2f destination)
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
    IO::logln("Switched behavior to {}!", behavior->name());

    _next_behavior = behavior;
}

} // namespace neko
