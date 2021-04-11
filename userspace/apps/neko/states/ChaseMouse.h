#pragma once

#include "neko/model/Behavior.h"

namespace neko
{

class ChaseMouse : public Behavior
{
private:
    Math::Vec2f _destination;

    Math::Vec2f pick_destination();

public:
    const char *name() override { return "ChaseMouse"; }

    ChaseMouse();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
