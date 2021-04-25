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

    Panel(int radius = 0);

    void paint(Graphic::Painter &painter, const Math::Recti &) override;
};

static inline RefPtr<Panel> panel() { return make<Panel>(); }

static inline RefPtr<Panel> panel(int radius) { return make<Panel>(radius); }

static inline RefPtr<Panel> panel(RefPtr<Element> child)
{
    auto panel = make<Panel>();
    panel->add(child);
    return panel;
}

static inline RefPtr<Panel> panel(int radius, RefPtr<Element> child)
{
    auto panel = make<Panel>(radius);
    panel->add(child);
    return panel;
}

} // namespace Widget
