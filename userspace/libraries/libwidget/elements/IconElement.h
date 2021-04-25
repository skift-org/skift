#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Element.h>

namespace Widget
{

class IconElement : public Element
{
private:
    RefPtr<Graphic::Icon> _icon;
    Graphic::IconSize _icon_size = Graphic::ICON_18PX;

public:
    IconElement(RefPtr<Graphic::Icon> icon, Graphic::IconSize size = Graphic::IconSize::ICON_18PX);

    void paint(Graphic::Painter &, const Math::Recti &) override;

    Math::Vec2i size() override;
};

static inline RefPtr<IconElement> icon(
    RefPtr<Graphic::Icon> icon,
    Graphic::IconSize size = Graphic::IconSize::ICON_18PX)
{
    return make<IconElement>(icon, size);
}

static inline RefPtr<IconElement> icon(
    String icon,
    Graphic::IconSize size = Graphic::IconSize::ICON_18PX)
{
    return make<IconElement>(Graphic::Icon::get(icon), size);
}

} // namespace Widget
