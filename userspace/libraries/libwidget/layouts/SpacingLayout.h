#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct SpacingLayout : public Element
{
private:
    Insetsi _spacing;

public:
    SpacingLayout(Insetsi spacing)
        : _spacing{spacing}
    {
    }

    virtual void layout()
    {
        if (childs().count() > 0)
        {
            childs()[0]->container(bound().shrinked(_spacing));
        }
    }

    virtual Math::Vec2i size()
    {
        if (childs().count() > 0)
        {
            return childs()[0]->compute_size() + _spacing.all();
        }
        else
        {
            return _spacing.all();
        }
    }
};

template <typename TElement>
static inline RefPtr<Element> spacing(Insetsi spacing, RefPtr<TElement> child)
{
    auto el = make<SpacingLayout>(spacing);
    el->add(child);
    return el;
}

} // namespace Widget
