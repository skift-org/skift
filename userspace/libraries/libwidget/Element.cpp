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

void Element::layout()
{
    for (auto &child : _children)
    {
        child->container(bound());
    }
}

void Element::relayout()
{
    layout();

    for (auto &child : children())
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
    if (_children.count() == 0)
    {
        return Math::Vec2i(0);
    }

    int width = 0;
    int height = 0;

    for (auto &child : _children)
    {
        Math::Vec2i child_size = child->compute_size();

        width = MAX(width, child_size.x());
        height = MAX(height, child_size.y());
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

/* --- Children ------------------------------------------------------------- */

Element *Element::at(Math::Vec2i position)
{
    Element *result = this;

    if (_flags & Element::GREEDY)
    {
        return result;
    }

    _children.foreach_reversed([&](RefPtr<Element> child) {
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
    if (child == nullptr)
    {
        return nullptr;
    }

    Assert::equal(child->_parent, nullptr);

    _children.push_back(child);

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

    _children.remove_value(child);
    should_relayout();
}

void Element::mount(Element &parent)
{
    _parent = &parent;
    mount(*parent.window());
}

void Element::mount(Window &window)
{
    _window = &window;

    for (auto &child : _children)
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

        for (auto &child : _children)
        {
            child->unmount();
        }
    }
}

void Element::clear()
{
    _children.clear();

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

    painter.push();
    paint(painter, rectangle);
    painter.pop();

    for (auto &child : _children)
    {
        auto local_rectangle = rectangle.offset(-child->origin());

        if (local_rectangle.colide_with(child->bound()))
        {
            child->repaint(painter, local_rectangle);
        }
    }

    if (Application::the().show_wireframe())
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
    if (!bound().colide_with(rectangle))
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
    _handlers[event_type] = std::move(handler);
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

    return Math::Vec2i(width, height);
}

} // namespace Widget
