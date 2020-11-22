#include <libwidget/Screen.h>

#include "neko/Neko.h"
#include "neko/states/ChaseMouse.h"
#include "neko/states/Wandering.h"

namespace neko
{

Vec2f Wandering::pick_destination()
{
    int x = random_uint32_max(&_random, Screen::bound().width() - Neko::SIZE * 2) + Neko::SIZE;
    int y = random_uint32_max(&_random, Screen::bound().height() - Neko::SIZE * 2) + Neko::SIZE;

    return {(float)x, (float)y};
}

Wandering::Wandering() :
    _random(random_create()),
    _destination(pick_destination()),
    _timer(random_uint32_max(&_random, 64) + 8)
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
    return animation_from_vector(neko.position().vector_to(_destination));
}

} // namespace neko
