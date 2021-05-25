#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct StackLayout : public Element
{
public:
    Math::Vec2i size() override
    {
        if (children().count() == 0)
        {
            return Math::Vec2i(0);
        }

        int width = 0;
        int height = 0;

        for (auto &child : children())
        {
            Math::Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());
        }

        return {width, height};
    }

    void layout() override
    {
        for (auto &child : children())
        {
            if (child->flags() & FILL)
            {
                auto b = bound();

                if (child->max_width() || child->max_height())
                {
                    if (child->max_width() > 0 && b.width() > child->max_width())
                    {
                        b = b.with_width(child->max_width());
                    }

                    if (child->max_height() > 0 && b.height() > child->max_height())
                    {
                        b = b.with_height(child->max_height());
                    }

                    child->container(b.centered_within(bound()));
                }
                else
                {
                    child->container(bound());
                }
            }
            else
            {
                auto size = child->compute_size();

                child->container(Math::Recti{size}.centered_within(bound()));
            }
        }
    }
};

static inline RefPtr<Element> stack(RefPtr<Element> child)
{
    auto layout = make<StackLayout>();
    layout->add(child);
    return layout;
}

static inline RefPtr<Element> stack(Vector<RefPtr<Element>> children)
{
    auto layout = make<StackLayout>();
    layout->add(children);
    return layout;
}

} // namespace Widget
