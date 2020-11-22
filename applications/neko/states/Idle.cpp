#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "neko/Neko.h"
#include "neko/states/Idle.h"
#include "neko/states/Itching.h"
#include "neko/states/Playing.h"
#include "neko/states/Surprised.h"
#include "neko/states/Wandering.h"
#include "neko/states/Yawning.h"

namespace neko
{

Idle::Idle() :
    _random(random_create()),
    _timer(random_uint32_max(&_random, 16) + 4)
{
}

void Idle::update(Neko &neko)
{
    if (_timer < neko.tick())
    {
        OwnPtr<Behavior> next[] = {
            own<Wandering>(),
            own<Yawning>(),
            own<Playing>(),
            own<Itching>(),
        };

        neko.behavior(random_pick(&_random, next, __array_length(next)));
    }
    else if (application_get_mouse_position().distance_to(neko.position()) > 64)
    {
        neko.behavior(own<Surprised>());
    }
} // namespace neko

Animation Idle::animation(Neko &neko)
{
    __unused(neko);

    return Animation::STOP;
}

} // namespace neko
