#pragma once

#include <libwidget/Component.h>

namespace Widget
{

class Placeholder : public Component
{
private:
    String _text;
    RefPtr<Graphic::Icon> _alert_icon;

public:
    Placeholder(Component *parent, String text);

    void paint(Graphic::Painter &, const Recti &) override;
};

} // namespace Widget
