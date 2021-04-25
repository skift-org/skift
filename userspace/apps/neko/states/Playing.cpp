#include <libwidget/Application.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/Idle.h"
#include "neko/states/Playing.h"
#include "neko/states/Surprised.h"

namespace neko
{

Playing::Playing()
    : _last_mouse_position(Widget::Application::the()->mouse_position())
{
}

void Playing::update(Neko &neko)
{
    auto new_mouse_position = Widget::Application::the()->mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }
    else if (neko.tick() > 8)
    {
        neko.behavior(own<Idle>());
    }
    else
    {
        _last_mouse_position = new_mouse_position;
        neko.did_update();
    }
}

Animation Playing::animation(Neko &)
{
    return Animations::JARE;
}

} // namespace neko
