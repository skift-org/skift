#pragma once

#include <libwidget/Component.h>

struct Spacer : public Component
{
    Spacer(Component *parent) : Component(parent)
    {
        flags(Component::FILL | Component::NO_MOUSE_HIT);
    }
};