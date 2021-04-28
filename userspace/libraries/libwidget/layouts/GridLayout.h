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

} // namespace Widget
