#pragma once

#include <libutils/Random.h>

#include "neko/model/Behavior.h"

namespace neko
{

class Wandering : public Behavior
{
private:
    Random _random;
    Math::Vec2f _destination;
    int _timer;

    Math::Vec2f pick_destination();

public:
    const char *name() override { return "Wandering"; }

    Wandering();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
