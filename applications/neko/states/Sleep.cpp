#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "neko/Neko.h"
#include "neko/states/Sleep.h"
#include "neko/states/Surprised.h"

namespace neko
{

Sleep::Sleep()
{
    _last_mouse_position = application_get_mouse_position();
}

void Sleep::update(Neko &neko)
{
    auto new_mouse_position = application_get_mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }

    _last_mouse_position = new_mouse_position;

    neko.did_update();
}

Animation Sleep::animation(Neko &neko)
{
    __unused(neko);

    return Animation::SLEEP;
}

} // namespace neko
