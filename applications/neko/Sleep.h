#pragma once

#include "neko/Behavior.h"

namespace neko
{

class Sleep : public Behavior
{
private:
    Vec2i _last_mouse_position;

public:
    Sleep();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
