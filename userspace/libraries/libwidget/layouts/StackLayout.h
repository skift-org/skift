#pragma once

#include <libwidget/Element.h>

namespace Widget
{

class StackLayout : public Element
{
public:
    Math::Vec2i size() override
    {
        if (childs().count() == 0)
        {
            return Math::Vec2i(0);
        }

        int width = 0;
        int height = 0;

        for (auto &child : childs())
        {
            Math::Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());
        }

        return {width, height};
    }

    void do_layout() override
    {
        for (auto &child : childs())
        {
            if (child->flags() & FILL)
            {
                auto bound = content();

                if (child->max_width() || child->max_height())
                {
                    if (child->max_width() > 0 && bound.width() > child->max_width())
                    {
                        bound = bound.with_width(child->max_width());
                    }

                    if (child->max_height() > 0 && bound.height() > child->max_height())
                    {
                        bound = bound.with_height(child->max_height());
                    }

                    child->container(bound.centered_within(content()));
                }
                else
                {
                    child->container(content());
                }
            }
            else
            {
                auto size = child->compute_size();

                child->container(Math::Recti{size}.centered_within(content()));
            }
        }
    }
};

static inline RefPtr<Element> stack(Vector<RefPtr<Element>> childs)
{
    auto layout = make<StackLayout>();
    layout->add(childs);
    return layout;
}

} // namespace Widget
