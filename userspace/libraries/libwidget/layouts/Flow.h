#pragma once

#include <libwidget/Element.h>

namespace Widget
{

static inline RefPtr<Element> vflow(Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(VFLOW(0));
    layout->add(childs);
    return layout;
}

static inline RefPtr<Element> vflow(int spacing, Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(VFLOW(spacing));
    layout->add(childs);
    return layout;
}

static inline RefPtr<Element> hflow(Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(HFLOW(0));
    layout->add(childs);
    return layout;
}

static inline RefPtr<Element> hflow(int spacing, Vector<RefPtr<Element>> childs)
{
    auto layout = make<Element>();
    layout->layout(HFLOW(spacing));
    layout->add(childs);
    return layout;
}

} // namespace Widget
