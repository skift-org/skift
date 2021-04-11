#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/Sleep.h"
#include "neko/states/Surprised.h"

namespace neko
{

Sleep::Sleep()
{
    _last_mouse_position = Widget::Application::mouse_position();
}

void Sleep::update(Neko &neko)
{
    auto new_mouse_position = Widget::Application::mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }
    else
    {
        _last_mouse_position = new_mouse_position;
        neko.did_update();
    }
}

Animation Sleep::animation(Neko &)
{
    return Animations::SLEEP;
}

} // namespace neko
