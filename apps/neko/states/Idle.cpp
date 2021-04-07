#include <libwidget/Application.h>
#include <libwidget/Screen.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/Idle.h"
#include "neko/states/Itching.h"
#include "neko/states/Playing.h"
#include "neko/states/Surprised.h"
#include "neko/states/Wandering.h"
#include "neko/states/Yawning.h"

namespace neko
{

Idle::Idle()
    : _random{},
      _timer(_random.next_u32(16) + 4)
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

        neko.behavior(_random.pick(next, AERAY_LENGTH(next)));
    }
    else if (Widget::Application::mouse_position().distance_to(neko.position()) > 64)
    {
        neko.behavior(own<Surprised>());
    }
} // namespace neko

Animation Idle::animation(Neko &)
{
    return Animations::STOP;
}

} // namespace neko
