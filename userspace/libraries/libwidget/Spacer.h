#pragma once

#include <libwidget/Component.h>

namespace Widget
{

struct Spacer : public Component
{
    Spacer()
    {
        flags(Component::FILL | Component::NO_MOUSE_HIT);
    }
};

} // namespace Widget
