#include <libwidget/Screen.h>

#include "neko/graphics/Animations.h"
#include "neko/model/Neko.h"
#include "neko/states/ChaseMouse.h"
#include "neko/states/Surprised.h"

namespace neko
{

Surprised::Surprised()
{
}

void Surprised::update(Neko &neko)
{
    if (neko.tick() > 2)
    {
        neko.behavior(own<ChaseMouse>());
    }
}

Animation Surprised::animation(Neko &)
{
    return Animations::AWAKE;
}

} // namespace neko
