#include <libwidget/Application.h>

#include "neko/Neko.h"
#include "neko/states/Itching.h"
#include "neko/states/Surprised.h"
#include "neko/states/Yawning.h"

namespace neko
{

Itching::Itching() :
    _last_mouse_position(application_get_mouse_position())
{
}

void Itching::update(Neko &neko)
{
    auto new_mouse_position = application_get_mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }
    else if (neko.tick() > 8)
    {
        neko.behavior(own<Yawning>());
    }

    _last_mouse_position = new_mouse_position;

    neko.did_update();
}

Animation Itching::animation(Neko &neko)
{
    __unused(neko);

    return Animation::KAKI;
}

} // namespace neko
