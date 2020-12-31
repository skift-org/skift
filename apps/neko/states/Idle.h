#pragma once

#include <libutils/Random.h>

#include "neko/model/Behavior.h"

namespace neko
{

class Idle :
    public Behavior
{
private:
    Random _random;
    int _timer;

public:
    const char *name() override { return "Idle"; }

    Idle();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
