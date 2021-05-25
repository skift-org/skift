#pragma once

#include <libmath/Random.h>

#include "neko/model/Behavior.h"

namespace neko
{

struct Idle :
    public Behavior
{
private:
    Math::Random _random;
    int _timer;

public:
    const char *name() override { return "Idle"; }

    Idle();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
