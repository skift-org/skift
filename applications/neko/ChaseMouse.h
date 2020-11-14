#pragma once


#include "neko/Behavior.h"

namespace neko
{

class ChaseMouse : public Behavior
{
private:
    Vec2f _destination;

    Vec2f pick_destination();

public:
    ChaseMouse();

    ~ChaseMouse() override;

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
