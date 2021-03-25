#include <assert.h>

#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libsystem/math/MinMax.h>
#include <libwidget/Application.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>
#include <libwidget/Widget.h>
#include <libwidget/Window.h>

void Widget::id(String id)
{
    _window->register_widget_by_id(id, this);
}

Color Widget::color(ThemeColorRole role)
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

    if (_colors[role])
    {
        return *_colors[role];
    }

    return _window->color(role);
}

void Widget::color(ThemeColorRole role, Color color)
{
    _colors[role] = color;

    should_repaint();
}

CursorState Widget::cursor()
{
    return _cursor;
}

void Widget::cursor(CursorState cursor)
{
    _cursor = cursor;

    window()->cursor(cursor);
}

Widget::Widget(Widget *parent)
{
    _enabled = true;
    _container = Recti(32, 32);

    if (parent)
    {
        _window = parent->_window;
        parent->add_child(this);
    }
}

Widget::~Widget()
{
    clear_children();

    if (_parent)
    {
        _parent->remove_child(this);
        _parent = nullptr;
    }

    if (_window)
    {
        _window->widget_removed(this);
        _window = nullptr;
    }
}

void Widget::do_layout()
{
    switch (_layout.type)
    {

    case Layout::STACK:
    {
        _childs.foreach ([this](Widget *child) {
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

                child->container(Recti{size}.centered_within(content()));
            }

            return Iteration::CONTINUE;
        });
    }
    break;

    case Layout::GRID:
    {
        int index = 0;

        _childs.foreach ([&](auto child) {
            int x = index % _layout.hcell;
            int y = index / _layout.hcell;

            Recti row = content().row(_layout.vcell, y, _layout.spacing.y());
            Recti column = row.column(_layout.hcell, x, _layout.spacing.x());

            child->container(column);
            index++;
            return Iteration::CONTINUE;
        });
    }
    break;

    case Layout::HGRID:
    {
        int index = 0;

        _childs.foreach ([&](auto child) {
            auto bound = content().column(_childs.count(), index, _layout.spacing.x());
            child->container(bound);
            index++;

            return Iteration::CONTINUE;
        });
    }
    break;

    case Layout::VGRID:
    {
        int index = 0;

        _childs.foreach ([&](auto child) {
            auto bound = content().row(_childs.count(), index, _layout.spacing.y());
            child->container(bound);
            index++;

            return Iteration::CONTINUE;
        });
    }
    break;

    case Layout::HFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_width = 0;

        int fill_child_count = 0;

        _childs.foreach ([&](auto child) {
            if (child->flags() & Widget::FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;

                if (child->flags() & Widget::SQUARE)
                {
                    fixed_child_total_width += content().height();
                }
                else
                {
                    fixed_child_total_width += child->compute_size().x();
                }
            }

            return Iteration::CONTINUE;
        });

        int usable_space =
            content().width() -
            _layout.spacing.x() * (_childs.count() - 1);

        int fill_child_total_width = MAX(0, usable_space - fixed_child_total_width);

        int fill_child_width = (fill_child_total_width) / MAX(1, fill_child_count);

        int current = content().x();

        _childs.foreach ([&](auto child) {
            if (child->flags() & Widget::FILL)
            {
                child->container(Recti(
                    current,
                    content().y(),
                    fill_child_width,
                    content().height()));

                current += fill_child_width + _layout.spacing.x();
            }
            else
            {
                if (child->flags() & Widget::SQUARE)
                {
                    child->container(Recti(
                        current,
                        content().y(),
                        content().height(),
                        content().height()));

                    current += content().height() + _layout.spacing.x();
                }
                else
                {
                    child->container(Recti(
                        current,
                        content().y(),
                        child->compute_size().x(),
                        content().height()));

                    current += child->compute_size().x() + _layout.spacing.x();
                }
            }

            return Iteration::CONTINUE;
        });
    }
    break;

    case Layout::VFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_height = 0;

        int fill_child_count = 0;

        _childs.foreach ([&](auto child) {
            if (child->flags() & Widget::FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;
                fixed_child_total_height += child->compute_size().y();
            }

            return Iteration::CONTINUE;
        });

        int usable_space = content().height() - _layout.spacing.y() * (_childs.count() - 1);

        int fill_child_total_height = MAX(0, usable_space - fixed_child_total_height);

        int fill_child_height = (fill_child_total_height) / MAX(1, fill_child_count);

        int current = content().y();

        _childs.foreach ([&](auto child) {
            if (child->flags() & Widget::FILL)
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
                if (child->flags() & Widget::SQUARE)
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

            return Iteration::CONTINUE;
        });
    }
    break;

    default:
        break;
    }
}

void Widget::relayout()
{
    do_layout();

    if (_childs.empty())
    {
        return;
    }

    _childs.foreach ([&](auto child) {
        child->relayout();
        return Iteration::CONTINUE;
    });
}

void Widget::should_relayout()
{
    if (_window)
    {
        _window->should_relayout();
    }
}

Vec2i Widget::size()
{
    if (_childs.count() == 0)
    {
        return Vec2i(0);
    }

    int width = 0;
    int height = 0;

    if (_layout.type == Layout::STACK)
    {
        _childs.foreach ([&](auto child) {
            Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());

            return Iteration::CONTINUE;
        });
    }
    else if (_layout.type == Layout::GRID)
    {
        _childs.foreach ([&](auto child) {
            Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());

            return Iteration::CONTINUE;
        });

        width = width * _layout.hcell;
        height = height * _layout.vcell;

        width += _layout.spacing.x() * _layout.hcell;
        height += _layout.spacing.y() * _layout.vcell;
    }
    else
    {
        _childs.foreach ([&](auto child) {
            Vec2i child_size = child->compute_size();

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
                width = MAX(width, child_size.x());
                height = MAX(height, child_size.y());
                break;
            }

            return Iteration::CONTINUE;
        });

        if (_layout.type == Layout::HFLOW || _layout.type == Layout::HGRID)
        {
            width += _layout.spacing.x() * (_childs.count() - 1);
        }

        if (_layout.type == Layout::VFLOW || _layout.type == Layout::VGRID)
        {
            height += _layout.spacing.y() * (_childs.count() - 1);
        }
    }

    return Vec2i(width, height);
}

/* --- Enable/ Disable state ------------------------------------------------ */

bool Widget::enabled() { return _enabled; }

bool Widget::disabled() { return !_enabled; }

void Widget::enable()
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

void Widget::disable()
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

void Widget::disable_if(bool condition)
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

void Widget::enable_if(bool condition)
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

Widget *Widget::child_at(Vec2i position)
{
    Widget *result = this;

    if (_flags & Widget::GREEDY)
    {
        return result;
    }

    _childs.foreach_reversed([&](Widget *child) {
        if (!(child->flags() & NO_MOUSE_HIT) && child->container().contains(position))
        {
            result = child->child_at(position - child->origin());

            return Iteration::STOP;
        }
        else
        {
            return Iteration::CONTINUE;
        }
    });

    return result;
}

void Widget::add_child(Widget *child)
{
    assert(child);
    assert(child->_parent == nullptr);

    child->_parent = this;
    child->_window = _window;
    _childs.push_back(child);

    should_relayout();
    should_repaint();
}

void Widget::remove_child(Widget *child)
{
    assert(child->_parent == this);
    _childs.remove_value(child);
    should_relayout();
}

void Widget::clear_children()
{
    while (_childs.any())
    {
        delete _childs.peek();
    }

    should_relayout();
    should_repaint();
}

/* --- Focus state ---------------------------------------------------------- */

bool Widget::focused()
{
    return _window && _window->has_keyboard_focus(this);
}

void Widget::focus()
{
    if (_window)
    {
        _window->focus_widget(this);
    }
}

/* --- Paint ---------------------------------------------------------------- */

void Widget::repaint(Painter &painter, Recti rectangle)
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
        painter.fill_insets(bound(), _insets, Colors::MAGENTA.with_alpha(0.25));
    }

    painter.push();
    paint(painter, rectangle);
    painter.pop();

    _childs.foreach ([&](auto child) {
        auto local_rectangle = rectangle.offset(-child->origin());

        if (local_rectangle.colide_with(child->bound()))
        {
            child->repaint(painter, local_rectangle);
        }

        return Iteration::CONTINUE;
    });

    if (Application::show_wireframe())
    {
        painter.draw_rectangle(bound(), Colors::CYAN.with_alpha(0.25));
    }

    painter.pop();
}

void Widget::should_repaint()
{
    should_repaint(bound());
}

void Widget::should_repaint(Recti rectangle)
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

void Widget::on(EventType event_type, EventHandler handler)
{
    assert(event_type < EventType::__COUNT);
    _handlers[event_type] = move(handler);
}

void Widget::dispatch_event(Event *event)
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

Vec2i Widget::compute_size()
{
    Vec2i size = this->size();

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

    return Vec2i(width, height);
}
