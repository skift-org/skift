#include <libwidget/Application.h>

#include "neko/Neko.h"
#include "neko/states/ChaseMouse.h"
#include "neko/states/Idle.h"

namespace neko
{

Vec2f ChaseMouse::pick_destination()
{
    return application_get_mouse_position();
}

ChaseMouse::ChaseMouse() :
    _destination(pick_destination())
{
}

void ChaseMouse::update(Neko &neko)
{
    auto distance = neko.position().distance_to(_destination);

    if (neko.tick() % 16)
    {
        _destination = pick_destination();
    }

    if (distance == 0)
    {
        neko.behavior(own<Idle>());
    }

    neko.move_to(_destination);
}

Animation ChaseMouse::animation(Neko &neko)
{
    return animation_from_vector(neko.position().vector_to(_destination));
}

} // namespace neko
