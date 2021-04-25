#pragma once

#include <libwidget/Element.h>

namespace Widget
{

static inline RefPtr<Element> grid(int hcells, int vcells, int hspacing, int vspacing, Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(GRID(hcells, vcells, hspacing, vspacing));
    layout->add(childs);
    return layout;
}

static inline RefPtr<Element> vgrid(int spacing, Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(VGRID(spacing));
    layout->add(childs);
    return layout;
}

static inline RefPtr<Element> hgrid(int spacing, Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(HGRID(spacing));
    layout->add(childs);
    return layout;
}

} // namespace Widget
