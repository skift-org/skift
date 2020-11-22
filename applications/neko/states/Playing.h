#pragma once

#include <libsystem/system/Random.h>

#include "neko/Behavior.h"

namespace neko
{

class Playing: public Behavior
{
private:
    Vec2i _last_mouse_position;

public:
    Playing();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
