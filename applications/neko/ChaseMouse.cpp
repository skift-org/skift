#include <libwidget/Application.h>

#include "neko/ChaseMouse.h"
#include "neko/Idle.h"
#include "neko/Neko.h"

namespace neko
{

Vec2f ChaseMouse::pick_destination()
{
    return application_get_mouse_position();
}

ChaseMouse::ChaseMouse() : _destination(pick_destination())
{
}

ChaseMouse::~ChaseMouse()
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
    return vector_to_move_animation(neko.position().vector_to(_destination));
}

} // namespace neko
