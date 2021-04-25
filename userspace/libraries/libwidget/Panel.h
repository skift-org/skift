#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class Panel : public Element
{
private:
    int _border_radius = 0;

public:
    void border_radius(int value) { _border_radius = value; }

    Panel();

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

} // namespace Widget
