#pragma once

#include "neko/model/Behavior.h"

namespace neko
{

class Surprised : public Behavior
{
private:
public:
    const char *name() override { return "Surprised"; }

    Surprised();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
