#include <libwidget/Screen.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/ChaseMouse.h"
#include "neko/states/Wandering.h"

namespace neko
{

Vec2f Wandering::pick_destination()
{
    int x = _random.next_u32(Widget::Screen::bound().width() - Neko::SIZE * 2) + Neko::SIZE;
    int y = _random.next_u32(Widget::Screen::bound().height() - Neko::SIZE * 2) + Neko::SIZE;

    return {(float)x, (float)y};
}

Wandering::Wandering()
    : _random{},
      _destination(pick_destination()),
      _timer(_random.next_u32(64) + 8)
{
}

void Wandering::update(Neko &neko)
{
    auto distance = neko.position().distance_to(_destination);

    if (distance == 0)
    {
        _destination = pick_destination();
    }

    if (_timer < neko.tick())
    {
        neko.behavior(own<ChaseMouse>());
    }
    else
    {
        neko.move_to(_destination);
    }
}

Animation Wandering::animation(Neko &neko)
{
    return Animations::from_vector(neko.position().vector_to(_destination));
}

} // namespace neko
