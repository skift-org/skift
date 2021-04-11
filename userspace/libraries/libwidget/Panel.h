#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class Panel : public Component
{
private:
    int _border_radius = 0;

public:
    void border_radius(int value) { _border_radius = value; }

    Panel(Component *parent);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace Widget
