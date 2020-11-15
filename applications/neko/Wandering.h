#pragma once

#include <libsystem/system/Random.h>

#include "neko/Behavior.h"

namespace neko
{

class Wandering : public Behavior
{
private:
    Random _random;
    Vec2f  _destination;
    int    _timer;

    Vec2f pick_destination();

public:
    Wandering();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
