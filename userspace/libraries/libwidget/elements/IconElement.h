#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Element.h>

namespace Widget
{

struct IconElement : public Element
{
private:
    RefPtr<Graphic::Icon> _icon;
    Graphic::IconSize _icon_size = Graphic::ICON_18PX;

public:
    IconElement(RefPtr<Graphic::Icon> icon, Graphic::IconSize size = Graphic::IconSize::ICON_18PX);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

WIDGET_BUILDER(IconElement, icon)

} // namespace Widget
