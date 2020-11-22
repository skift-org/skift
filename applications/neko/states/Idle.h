#pragma once

#include <libsystem/system/Random.h>

#include "neko/Behavior.h"

namespace neko
{

class Idle: public Behavior
{
private:
    Random _random;
    int _timer;

public:
    Idle();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
