#include <libgraphic/Font.h>
#include <libgraphic/Painter.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/io/Stream.h>
#include <libsystem/math/MinMax.h>
#include <libwidget/Application.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>
#include <libwidget/Widget.h>
#include <libwidget/Window.h>

void Widget::id(const char *id)
{
    window_register_widget_by_id(_window, id, this);
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

    if (_colors[role].overwritten)
    {
        return _colors[role].color;
    }

    return window_get_color(_window, role);
}

void Widget::color(ThemeColorRole role, Color color)
{
    _colors[role].overwritten = true;
    _colors[role].color = color;

    should_repaint();
}

Widget::Widget(Widget *parent)
{
    _enabled = true;
    _childs = list_create();
    _bound = Rectangle(32, 32);

    if (parent)
    {
        _window = parent->_window;
        parent->add_child(this);
    }
}

Widget::~Widget()
{
    clear_childs();

    list_destroy(_childs);

    if (_parent)
        _parent->remove_child(this);

    if (_window)
        window_widget_removed(_window, this);
}

void Widget::paint(Painter &painter, Rectangle rectangle)
{
    __unused(painter);
    __unused(rectangle);
}

void Widget::event(Event *event)
{
    __unused(event);
}

void Widget::do_vhgrid_layout(Layout layout, Dimension dim)
{
    Dimension ivdim = dimension_invert_xy(dim);

    int current = content_bound().position().component(dim);
    int spacing = layout.spacing.component(dim);
    int size = content_bound().size().component(dim);
    int used_space_without_spacing = size - (spacing * (_childs->count() - 1));
    int child_size = used_space_without_spacing / _childs->count();
    int used_space_with_spacing = child_size * _childs->count() + (spacing * (_childs->count() - 1));
    int correction_space = size - used_space_with_spacing;

    list_foreach(Widget, child, _childs)
    {
        int current_child_size = MAX(1, child_size);

        if (correction_space > 0)
        {
            current_child_size++;
            correction_space--;
        }

        if (dim == Dimension::X)
        {
            child->bound(Rectangle(
                current,
                content_bound().position().component(ivdim),
                current_child_size,
                content_bound().size().component(ivdim)));
        }
        else
        {
            child->bound(Rectangle(
                content_bound().position().component(ivdim),
                current,
                content_bound().size().component(ivdim),
                current_child_size));
        }

        current += current_child_size + spacing;
    }
}

void Widget::do_layout()
{
    switch (_layout.type)
    {
    case LAYOUT_STACK:
        list_foreach(Widget, child, _childs)
        {
            child->bound(content_bound());
        }
        break;
    case LAYOUT_GRID:
    {
        int originX = content_bound().x();
        int originY = content_bound().y();

        int child_width = (content_bound().width() - (_layout.spacing.x() * (_layout.hcell - 1))) / _layout.hcell;
        int child_height = (content_bound().height() - (_layout.spacing.y() * (_layout.vcell - 1))) / _layout.vcell;

        int index = 0;
        list_foreach(Widget, child, _childs)
        {
            int x = index % _layout.hcell;
            int y = index / _layout.hcell;

            child->bound(Rectangle(
                originX + x * (child_width + _layout.spacing.x()),
                originY + y * (child_height + _layout.spacing.y()),
                child_width,
                child_height));

            index++;
        }
    }
    break;

    case LAYOUT_HGRID:
        do_vhgrid_layout(_layout, Dimension::X);
        break;

    case LAYOUT_VGRID:
        do_vhgrid_layout(_layout, Dimension::Y);
        break;

    case LAYOUT_HFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_width = 0;

        int fill_child_count = 0;

        list_foreach(Widget, child, _childs)
        {
            if (child->attributes() & LAYOUT_FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;
                fixed_child_total_width += child->compute_size().x();
            }
        }

        int usable_space =
            content_bound().width() -
            _layout.spacing.x() * (_childs->count() - 1);

        int fill_child_total_width = MAX(0, usable_space - fixed_child_total_width);

        int fill_child_width = (fill_child_total_width) / MAX(1, fill_child_count);

        int current = content_bound().x();

        list_foreach(Widget, child, _childs)
        {
            if (child->attributes() & LAYOUT_FILL)
            {
                child->bound(Rectangle(
                    current,
                    content_bound().y(),
                    fill_child_width,
                    content_bound().height()));

                current += fill_child_width + _layout.spacing.x();
            }
            else
            {
                child->bound(Rectangle(
                    current,
                    content_bound().y(),
                    child->compute_size().x(),
                    content_bound().height()));

                current += child->compute_size().x() + _layout.spacing.x();
            }
        }
    }
    break;

    case LAYOUT_VFLOW:
    {
        int fixed_child_count = 0;
        int fixed_child_total_height = 0;

        int fill_child_count = 0;

        list_foreach(Widget, child, _childs)
        {
            if (child->attributes() & LAYOUT_FILL)
            {
                fill_child_count++;
            }
            else
            {
                fixed_child_count++;
                fixed_child_total_height += child->compute_size().y();
            }
        }

        int usable_space =
            content_bound().height() -
            _layout.spacing.y() * (_childs->count() - 1);

        int fill_child_total_height = MAX(0, usable_space - fixed_child_total_height);

        int fill_child_height = (fill_child_total_height) / MAX(1, fill_child_count);

        int current = content_bound().y();

        list_foreach(Widget, child, _childs)
        {
            if (child->attributes() & LAYOUT_FILL)
            {
                child->bound(Rectangle(
                    content_bound().x(),
                    current,
                    content_bound().width(),
                    fill_child_height));

                current += fill_child_height + _layout.spacing.y();
            }
            else
            {
                child->bound(Rectangle(
                    content_bound().x(),
                    current,
                    content_bound().width(),
                    child->compute_size().y()));

                current += child->compute_size().y() + _layout.spacing.y();
            }
        }
    }
    break;

    default:
        break;
    }
}

void Widget::relayout()
{
    do_layout();

    if (_childs->count() == 0)
        return;

    list_foreach(Widget, child, _childs)
    {
        child->relayout();
    }
}

void Widget::should_relayout()
{
    if (_window)
    {
        window_schedule_layout(_window);
    }
}

Vec2i Widget::size()
{
    int width = 0;
    int height = 0;

    if (_layout.type == LAYOUT_STACK)
    {

        width = _min_width;
        height = _min_height;

        list_foreach(Widget, child, _childs)
        {
            Vec2i child_size = child->compute_size();

            width = MAX(width, child_size.x());
            height = MAX(height, child_size.y());
        }
    }
    else
    {
        list_foreach(Widget, child, _childs)
        {
            Vec2i child_size = child->compute_size();

            switch (_layout.type)
            {
            case LAYOUT_HFLOW:
            case LAYOUT_HGRID:
                width += child_size.x();
                height = MAX(height, child_size.y());
                break;

            case LAYOUT_VFLOW:
            case LAYOUT_VGRID:
                width = MAX(width, child_size.x());
                height += child_size.y();
                break;

            default:
                width = MAX(width, child_size.x());
                height = MAX(height, child_size.y());
                break;
            }
        }

        if (_layout.type == LAYOUT_HFLOW || _layout.type == LAYOUT_HGRID)
        {
            width += _layout.spacing.x() * (_childs->count() - 1);
        }

        if (_layout.type == LAYOUT_VFLOW || _layout.type == LAYOUT_VGRID)
        {
            height += _layout.spacing.y() * (_childs->count() - 1);
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
        should_repaint();
    }
}

void Widget::disable()
{
    if (enabled())
    {
        _enabled = false;
        should_repaint();
    }
}

void Widget::disable_if(bool condition)
{
    if (condition)
        disable();
    else
        enable();
}

void Widget::enable_if(bool condition)
{
    if (condition)
        enable();
    else
        disable();
}

/* --- Childs --------------------------------------------------------------- */

Widget *Widget::child_at(Vec2i position)
{
    if (_layout_attributes & LAYOUT_GREEDY)
    {
        return this;
    }

    list_foreach(Widget, child, _childs)
    {
        if (child->bound().containe(position))
        {
            return child->child_at(position);
        }
    }

    return this;
}

void Widget::add_child(Widget *child)
{
    assert(child);
    assert(child->_parent == nullptr);

    child->_parent = this;
    child->_window = _window;
    list_pushback(_childs, child);

    should_relayout();
}

void Widget::remove_child(Widget *child)
{
    assert(child->_parent == this);

    child->_parent = nullptr;
    child->_window = nullptr;
    list_remove(_childs, child);

    should_relayout();
}

void Widget::clear_childs()
{
    Widget *child = (Widget *)list_peek(_childs);
    while (child)
    {
        delete child;
        child = (Widget *)list_peek(_childs);
    }
}

/* --- Focus state ---------------------------------------------------------- */

bool Widget::focused()
{
    return _window && _window->focused_widget == this;
}

void Widget::focus()
{
    if (_window)
        window_set_focused_widget(_window, this);
}

/* --- Paint ---------------------------------------------------------------- */

void Widget::repaint(Painter &painter, Rectangle rectangle)
{
    if (bound().width() == 0 || bound().height() == 0)
        return;

    painter.push();
    painter.clip(bound());

    if (application_is_debbuging_layout())
        painter.fill_insets(bound(), _insets, ALPHA(COLOR_MAGENTA, 0.25));

    painter.push();
    paint(painter, rectangle);
    painter.pop();

    list_foreach(Widget, child, _childs)
    {
        if (rectangle.colide_with(child->bound()))
        {
            child->repaint(painter, rectangle);
        }
    }

    if (application_is_debbuging_layout())
        painter.draw_rectangle(bound(), ALPHA(COLOR_CYAN, 0.25));

    painter.pop();
}

void Widget::should_repaint()
{
    if (_window)
    {
        window_schedule_update(_window, bound());
    }
}

void Widget::should_repaint(Rectangle rectangle)
{
    if (_window)
    {
        window_schedule_update(_window, rectangle);
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
    this->event(event);

    if (!event->accepted && _handlers[event->type])
    {
        event->accepted = true;
        _handlers[event->type](event);
    }

    if (!event->accepted && _parent)
    {
        _parent->dispatch_event(event);
    }
}

static RefPtr<Font> _widget_font = nullptr;
RefPtr<Font> Widget::font()
{
    if (_widget_font == nullptr)
    {
        _widget_font = Font::create("sans").take_value();
    }

    return _widget_font;
}

Vec2i Widget::compute_size()
{
    Vec2i size = this->size();

    int width = size.x();
    int height = size.y();

    width += _insets.left();
    width += _insets.right();

    height += _insets.top();
    height += _insets.bottom();

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

    return Vec2i(width, height);
}
