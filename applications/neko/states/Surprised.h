#pragma once

#include <libsystem/system/Random.h>

#include "neko/Behavior.h"

namespace neko
{

class Surprised : public Behavior
{
private:
public:
    Surprised();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
