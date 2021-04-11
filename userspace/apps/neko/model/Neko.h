#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libutils/Observable.h>

#include "neko/graphics/Sprites.h"
#include "neko/model/Behavior.h"

namespace neko
{

class Neko : public Observable<Neko>
{
private:
    int _tick = 0;
    Math::Vec2f _position;
    RefPtr<Graphic::Bitmap> _sprites;

    OwnPtr<Behavior> _behavior;
    OwnPtr<Behavior> _next_behavior;

    Math::Recti sprite();

public:
    static constexpr int SIZE = 32;
    static constexpr int SPEED = 16;

    int tick() { return _tick; }

    Math::Vec2f position() { return _position; }

    Neko(Math::Vec2f starting_position);

    void update();

    void paint(Graphic::Painter &painter);

    void move_to(Math::Vec2f position);

    void behavior(OwnPtr<Behavior>);
};

} // namespace neko
