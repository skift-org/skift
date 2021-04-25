#include <assert.h>

#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libmath/MinMax.h>
#include <libwidget/Application.h>
#include <libwidget/Element.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

namespace Widget
{

Graphic::Color Element::color(ThemeColorRole role)
{
    if (!enabled() || (_parent && !_parent->enabled()))
    {
        if (role == THEME_FOREGROUND)
        {
            role = THEME_FOREGROUND_INACTIVE;
        }

        if (role == THEME_SELECTION)
        {
            role = THEME_SELECTION_INACTIVE;
        }

        if (role == THEME_ACCENT)
        {
            role = THEME_ACCENT_INACTIVE;
        }
    }

    if (!_window)
    {
        return Graphic::Colors::MAGENTA;
    }

    return _colors[role].unwrap_or(_window->color(role));
}

void Element::color(ThemeColorRole role, Graphic::Color color)
{
    _colors[role] = color;

    should_repaint();
}

CursorState Element::cursor()
{
    return _cursor;
}

void Element::cursor(CursorState cursor)
{
    _cursor = cursor;

    window()->cursor(cursor);
}

Element::Element()
{
    _enabled = true;
    _container = Math::Recti(32, 32);
}

Element::~Element()
{
    if (_window)
    {
        _window->widget_removed(this);
        _window = nullptr;
    }
}

void Element::do_layout()
{
    switch (_layout.type)
    {

    case Layout::STACK:
    {
        for (auto &child : _childs)
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
    break;

    case Layout::GRID:
    {
        int index = 0;

        for (auto &child : _childs)
        {
            int x = index % _layout.hcell;
            int y = index / _layout.hcell;

            Math::Recti row = content().row(_layout.vcell, y, _layout.spacing.y());
            Math::Recti column = row.column(_layout.hcell, x, _layout.spacing.x());

            child->container(column);
            index++;
        }
    }
    break;

    case Layout::HGRID:
    {
        int index = 0;

        for (auto &child : _childs)
        {
            auto bound = content().column(_childs.count(), index, _layout.spacing.x());
            child->container(bound);
            index++;
        }
    }
    break;

    case Layout::VGRID:
    {
        int index = 0;

        for (auto &child : _childs)
        {
            auto bound = content().row(_childs.count(), index, _layout.spacing.y());
            child->container(bound);
            index++;
        }
    }
    break;

    case Layout::HFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_width = 0;

        int fill_child_count = 0;

        for (auto &child : _childs)
        {
            if (child->flags() & Element::FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;

                if (child->flags() & Element::SQUARE)
                {
                    fixed_child_total_width += content().height();
                }
                else
                {
                    fixed_child_total_width += child->compute_size().x();
                }
            }
        }

        int usable_space =
            content().width() -
            _layout.spacing.x() * (_childs.count() - 1);

        int fill_child_total_width = MAX(0, usable_space - fixed_child_total_width);

        int fill_child_width = (fill_child_total_width) / MAX(1, fill_child_count);

        int current = content().x();

        for (auto &child : _childs)
        {
            if (child->flags() & Element::FILL)
            {
                child->container(Math::Recti(
                    current,
                    content().y(),
                    fill_child_width,
                    content().height()));

                current += fill_child_width + _layout.spacing.x();
            }
            else
            {
                if (child->flags() & Element::SQUARE)
                {
                    child->container(Math::Recti(
                        current,
                        content().y(),
                        content().height(),
                        content().height()));

                    current += content().height() + _layout.spacing.x();
                }
                else
                {
                    child->container(Math::Recti(
                        current,
                        content().y(),
                        child->compute_size().x(),
                        content().height()));

                    current += child->compute_size().x() + _layout.spacing.x();
                }
            }
        }
    }
    break;

    case Layout::VFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_height = 0;
        int fill_child_count = 0;

        for (auto &child : _childs)
        {
            if (child->flags() & Element::FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;
                fixed_child_total_height += child->compute_size().y();
            }
        }

        int usable_space = content().height() - _layout.spacing.y() * (_childs.count() - 1);

        int fill_child_total_height = MAX(0, usable_space - fixed_child_total_height);

        int fill_child_height = (fill_child_total_height) / MAX(1, fill_child_count);

        int current = content().y();

        for (auto &child : _childs)
        {
            if (child->flags() & Element::FILL)
            {
                child->container({
                    content().x(),
                    current,
                    content().width(),
                    fill_child_height,
                });

                current += fill_child_height + _layout.spacing.y();
            }
            else
            {
                if (child->flags() & Element::SQUARE)
                {
                    child->container({
                        content().x(),
                        current,
                        content().width(),
                        content().width(),
                    });

                    current += content().width() + _layout.spacing.y();
                }
                else
                {
                    child->container({
                        content().x(),
                        current,
                        content().width(),
                        child->compute_size().y(),
                    });

                    current += child->compute_size().y() + _layout.spacing.y();
                }
            }
        }
    }
    break;

    default:
        break;
    }
}

void Element::relayout()
{
    do_layout();

    if (_childs.empty())
    {
        return;
    }

    for (auto &child : _childs)
    {
        child->relayout();
    }
}

void Element::should_relayout()
{
    if (_window)
    {
        _window->should_relayout();
    }
}

Math::Vec2i Element::size()
{
    if (_childs.count() == 0)
    {
        return Math::Vec2i(0);
    }

    int width = 0;
    int height = 0;

    if (_layout.type == Layout::STACK)
    {
        for (auto &child : _childs)
        {
            Math::Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());
        }
    }
    else if (_layout.type == Layout::GRID)
    {
        for (auto &child : _childs)
        {
            Math::Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());
        }

        width = width * _layout.hcell;
        height = height * _layout.vcell;

        width += _layout.spacing.x() * _layout.hcell;
        height += _layout.spacing.y() * _layout.vcell;
    }
    else
    {
        for (auto &child : _childs)
        {
            Math::Vec2i child_size = child->compute_size();

            switch (_layout.type)
            {
            case Layout::HFLOW:
            case Layout::HGRID:
                width += child_size.x();
                height = MAX(height, child_size.y());
                break;

            case Layout::VFLOW:
            case Layout::VGRID:
                width = MAX(width, child_size.x());
                height += child_size.y();
                break;

            default:
                ASSERT_NOT_REACHED();
            }
        }

        if (_layout.type == Layout::HFLOW || _layout.type == Layout::HGRID)
        {
            width += _layout.spacing.x() * (_childs.count() - 1);
        }

        if (_layout.type == Layout::VFLOW || _layout.type == Layout::VGRID)
        {
            height += _layout.spacing.y() * (_childs.count() - 1);
        }
    }

    return {width, height};
}

/* --- Enable/ Disable state ------------------------------------------------ */

bool Element::enabled() { return _enabled; }

bool Element::disabled() { return !_enabled; }

void Element::enable()
{
    if (disabled())
    {
        _enabled = true;

        Event e;
        e.type = Event::WIDGET_ENABLE;
        event(&e);

        should_repaint();
    }
}

void Element::disable()
{
    if (enabled())
    {
        _enabled = false;

        Event e;
        e.type = Event::WIDGET_DISABLE;
        event(&e);

        should_repaint();
    }
}

void Element::disable_if(bool condition)
{
    if (condition)
    {
        disable();
    }
    else
    {
        enable();
    }
}

void Element::enable_if(bool condition)
{
    if (condition)
    {
        enable();
    }
    else
    {
        disable();
    }
}

/* --- Childs --------------------------------------------------------------- */

Element *Element::at(Math::Vec2i position)
{
    Element *result = this;

    if (_flags & Element::GREEDY)
    {
        return result;
    }

    _childs.foreach_reversed([&](RefPtr<Element> child) {
        if (!(child->flags() & NO_MOUSE_HIT) && child->container().contains(position))
        {
            result = child->at(position - child->origin());

            return Iteration::STOP;
        }
        else
        {
            return Iteration::CONTINUE;
        }
    });

    return result;
}

RefPtr<Element> Element::add(RefPtr<Element> child)
{
    Assert::not_null(child);
    Assert::equal(child->_parent, nullptr);

    _childs.push_back(child);

    if (_window != nullptr)
    {
        child->mount(*this);
    }

    should_relayout();
    should_repaint();

    return child;
}

void Element::del(RefPtr<Element> child)
{
    Assert::equal(child->_parent, this);

    child->unmount();

    _childs.remove_value(child);
    should_relayout();
}

void Element::mount(Element &parent)
{
    _parent = &parent;
    _window = parent.window();

    for (auto &child : _childs)
    {
        child->mount(*this);
    }

    mounted();
}

void Element::unmount()
{
    if (_parent)
    {
        _parent = nullptr;
        _window = nullptr;

        for (auto &child : _childs)
        {
            child->unmount();
        }
    }
}

void Element::clear()
{
    _childs.clear();

    should_relayout();
    should_repaint();
}

/* --- Focus state ---------------------------------------------------------- */

bool Element::focused()
{
    return _window && _window->has_keyboard_focus(this);
}

void Element::focus()
{
    if (_window)
    {
        _window->focus_widget(this);
    }
}

/* --- Paint ---------------------------------------------------------------- */

void Element::repaint(Graphic::Painter &painter, Math::Recti rectangle)
{
    if (bound().width() == 0 || bound().height() == 0)
    {
        return;
    }

    painter.push();
    painter.transform(origin());
    painter.clip(bound());

    if (Application::show_wireframe())
    {
        painter.fill_insets(bound(), _insets, Graphic::Colors::MAGENTA.with_alpha(0.25));
    }

    painter.push();
    paint(painter, rectangle);
    painter.pop();

    for (auto &child : _childs)
    {
        auto local_rectangle = rectangle.offset(-child->origin());

        if (local_rectangle.colide_with(child->bound()))
        {
            child->repaint(painter, local_rectangle);
        }
    }

    if (Application::show_wireframe())
    {
        painter.draw_rectangle(bound(), Graphic::Colors::CYAN.with_alpha(0.25));
    }

    painter.pop();
}

void Element::should_repaint()
{
    should_repaint(bound());
}

void Element::should_repaint(Math::Recti rectangle)
{
    if (!overflow().colide_with(rectangle))
    {
        return;
    }

    // Convert to the parent coordinate space.
    rectangle = rectangle.offset(origin());

    if (_parent)
    {
        _parent->should_repaint(rectangle);
    }
    else if (_window)
    {
        _window->should_repaint(rectangle);
    }
}

/* --- Events ----------------------------------------------------------------*/

void Element::on(EventType event_type, EventHandler handler)
{
    assert(event_type < EventType::__COUNT);
    _handlers[event_type] = move(handler);
}

void Element::dispatch_event(Event *event)
{
    if (enabled())
    {
        this->event(event);

        if (!event->accepted && _handlers[event->type])
        {
            event->accepted = true;
            _handlers[event->type](event);
        }
    }

    if (!event->accepted && _parent)
    {
        _parent->dispatch_event(event);
    }
}

Math::Vec2i Element::compute_size()
{
    Math::Vec2i size = this->size();

    int width = size.x();
    int height = size.y();

    width += _insets.left() + _insets.right();
    height += _insets.top() + _insets.bottom();

    if (_max_width)
    {
        width = MIN(width, _max_width);
    }

    if (_max_height)
    {
        height = MIN(height, _max_height);
    }

    if (_min_width)
    {
        width = MAX(width, _min_width);
    }

    if (_min_height)
    {
        height = MAX(height, _min_height);
    }

    width += _outsets.left() + _outsets.right();
    height += _outsets.top() + _outsets.bottom();

    return Math::Vec2i(width, height);
}

} // namespace Widget
