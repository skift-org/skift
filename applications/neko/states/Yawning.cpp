#include <libwidget/Application.h>

#include "neko/Neko.h"
#include "neko/states/Sleep.h"
#include "neko/states/Surprised.h"
#include "neko/states/Yawning.h"

namespace neko
{

Yawning::Yawning() :
    _last_mouse_position(application_get_mouse_position())
{
}

void Yawning::update(Neko &neko)
{
    auto new_mouse_position = application_get_mouse_position();

    if (_last_mouse_position.distance_to(new_mouse_position) > 16)
    {
        neko.behavior(own<Surprised>());
    }
    else if (neko.tick() > 4)
    {
        neko.behavior(own<Sleep>());
    }

    _last_mouse_position = new_mouse_position;

    neko.did_update();
}

Animation Yawning::animation(Neko &neko)
{
    __unused(neko);

    return Animation::AKUBI;
}

} // namespace neko
