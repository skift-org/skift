#pragma once

#include <libwidget/Element.h>

namespace Widget
{

struct FlowLayout : public Element
{
private:
    int _spacing = 0;
    bool _horizontal = false;

public:
    FlowLayout(int spacing = 0, bool horizontal = false)
        : _spacing{spacing},
          _horizontal{horizontal}
    {
    }

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

            width = _horizontal ? width + child_size.x() : MAX(width, child_size.x());
            height = _horizontal ? MAX(height, child_size.y()) : height + child_size.y();
        }

        if (_horizontal)
        {
            width += _spacing * (children().count() - 1);
        }
        else
        {
            height += _spacing * (children().count() - 1);
        }

        return {width, height};
    }

    void layout() override
    {
        int fill_child_count = 0;
        int fixed_child_total_size = 0;

        for (auto &child : children())
        {
            if (child->flags() & Element::FILL)
            {
                fill_child_count++;
            }
            else if (child->flags() & Element::SQUARE)
            {
                fixed_child_total_size += _horizontal ? bound().height() : bound().width();
            }
            else
            {
                fixed_child_total_size += _horizontal ? child->compute_size().x() : child->compute_size().y();
            }
        }

        int usable_space = (_horizontal ? bound().width() : bound().height()) - _spacing * (children().count() - 1);
        int fill_child_total_size = MAX(0, usable_space - fixed_child_total_size);
        int fill_child_size = (fill_child_total_size) / MAX(1, fill_child_count);
        int current = _horizontal ? bound().x() : bound().y();

        for (auto &child : children())
        {
            if (child->flags() & Element::FILL)
            {
                child->container({
                    _horizontal ? current : bound().y(),
                    _horizontal ? bound().y() : current,
                    _horizontal ? fill_child_size : bound().width(),
                    _horizontal ? bound().height() : fill_child_size,
                });

                current += fill_child_size + _spacing;
            }
            else if (child->flags() & Element::SQUARE)
            {
                child->container({
                    _horizontal ? current : bound().x(),
                    _horizontal ? bound().y() : current,
                    _horizontal ? bound().height() : bound().width(),
                    _horizontal ? bound().height() : bound().width(),
                });

                current += (_horizontal ? bound().height() : bound().width()) + _spacing;
            }
            else
            {
                child->container({
                    _horizontal ? current : bound().x(),
                    _horizontal ? bound().y() : current,
                    _horizontal ? child->compute_size().x() : bound().width(),
                    _horizontal ? bound().height() : child->compute_size().y(),
                });

                current += (_horizontal ? child->compute_size().x() : child->compute_size().y()) + _spacing;
            }
        }
    }
};

static inline RefPtr<Element> vflow(Vector<RefPtr<Element>> children)
{
    auto layout = make<FlowLayout>(0, false);
    layout->add(children);
    return layout;
}

static inline RefPtr<Element> vflow(int spacing, Vector<RefPtr<Element>> children)
{
    auto layout = make<FlowLayout>(spacing, false);
    layout->add(children);
    return layout;
}

static inline RefPtr<Element> hflow(Vector<RefPtr<Element>> children)
{
    auto layout = make<FlowLayout>(0, true);
    layout->add(children);
    return layout;
}

static inline RefPtr<Element> hflow(int spacing, Vector<RefPtr<Element>> children)
{
    auto layout = make<FlowLayout>(spacing, true);
    layout->add(children);
    return layout;
}

} // namespace Widget
