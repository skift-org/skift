#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class Placeholder : public Element
{
private:
    String _text;
    RefPtr<Graphic::Icon> _alert_icon;

public:
    Placeholder(String text);

    void paint(Graphic::Painter &, const Math::Recti &) override;
};

} // namespace Widget
