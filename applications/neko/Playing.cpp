#include <libwidget/Application.h>

#include "neko/Idle.h"
#include "neko/Neko.h"
#include "neko/Playing.h"
#include "neko/Surprised.h"

namespace neko
{

Playing::Playing()
    : _last_mouse_position(application_get_mouse_position())
{
}

Playing::~Playing()
{
}

void Playing::update(Neko &neko)
{
    auto new_mouse_position = application_get_mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }
    else if (neko.tick() > 8)
    {
        neko.behavior(own<Idle>());
    }

    _last_mouse_position = new_mouse_position;

    neko.did_update();
}

Animation Playing::animation(Neko &neko)
{
    __unused(neko);

    return Animation::JARE;
}

} // namespace neko
