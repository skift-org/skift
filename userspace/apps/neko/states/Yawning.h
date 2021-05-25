#pragma once

#include <libmath/Random.h>

#include "neko/model/Behavior.h"

namespace neko
{

struct Yawning :
    public Behavior
{
private:
    Math::Vec2i _last_mouse_position;

public:
    const char *name() override { return "Yawning"; }

    Yawning();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
