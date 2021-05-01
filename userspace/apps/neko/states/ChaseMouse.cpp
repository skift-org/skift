#include <libwidget/Application.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/ChaseMouse.h"
#include "neko/states/Idle.h"

namespace neko
{

Math::Vec2f ChaseMouse::pick_destination()
{
    return Widget::Application::the().mouse_position();
}

ChaseMouse::ChaseMouse()
    : _destination(pick_destination())
{
}

void ChaseMouse::update(Neko &neko)
{
    _destination = pick_destination();

    auto distance = neko.position().distance_to(_destination);

    if (distance)
    {
        neko.move_to(_destination);
    }
    else
    {
        neko.behavior(own<Idle>());
    }
}

Animation ChaseMouse::animation(Neko &neko)
{
    return Animations::from_vector(neko.position().vector_to(_destination));
}

} // namespace neko
