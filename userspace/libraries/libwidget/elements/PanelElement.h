#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class PanelElement : public Element
{
private:
    int _border_radius = 0;

public:
    void border_radius(int value) { _border_radius = value; }

    PanelElement(int radius = 0);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

static inline RefPtr<PanelElement> panel()
{
    return make<PanelElement>();
}

static inline RefPtr<PanelElement> panel(int radius)
{
    return make<PanelElement>(radius);
}

static inline RefPtr<PanelElement> panel(RefPtr<Element> child)
{
    auto panel = make<PanelElement>();
    panel->add(child);
    return panel;
}

static inline RefPtr<PanelElement> panel(int radius, RefPtr<Element> child)
{
    auto panel = make<PanelElement>(radius);
    panel->add(child);
    return panel;
}

} // namespace Widget
