#include <libgraphic/Painter.h>
#include <libwidget/Window.h>

#include "file-explorer/Breadcrumb.h"

Breadcrumb::Breadcrumb(Widget *parent, Path *path) : Widget(parent)
{
    _path = path_clone(path);
    _icon_computer = Icon::get("laptop");
    _icon_expand = Icon::get("chevron-right");
}

Breadcrumb::~Breadcrumb()
{
    path_destroy(_path);
}

void Breadcrumb::navigate(Path *path)
{
    path_destroy(_path);
    _path = path_clone(path);
    should_repaint();
}

void Breadcrumb::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    int current = 0;

    Rectangle computer_icon_bound(
        widget_get_bound(this).x(),
        widget_get_bound(this).y(),
        _icon_computer->bound(ICON_18PX).width() + 16,
        widget_get_bound(this).height());

    painter.blit_icon(
        *_icon_computer,
        ICON_18PX,
        _icon_computer->bound(ICON_18PX).centered_within(computer_icon_bound),
        widget_get_color(this, THEME_FOREGROUND));

    current += computer_icon_bound.width();

    if (path_element_count(_path) != 0)
    {
        Rectangle expand_icon_bound(
            widget_get_bound(this).x() + current,
            widget_get_bound(this).y(),
            _icon_expand->bound(ICON_18PX).width(),
            widget_get_bound(this).height());

        painter.blit_icon(
            *_icon_expand,
            ICON_18PX,
            _icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
            widget_get_color(this, THEME_FOREGROUND));

        current += expand_icon_bound.width();
    }

    for (size_t i = 0; i < path_element_count(_path); i++)
    {
        const char *element = path_peek_at(_path, i);

        int text_width = widget_font()->mesure_string(element).width();

        Rectangle element_bound(
            widget_get_bound(this).x() + current,
            widget_get_bound(this).y(),
            text_width,
            widget_get_bound(this).height());

        painter.draw_string(
            *widget_font(),
            element,
            element_bound.position() + Vec2i(0, 19),
            widget_get_color(this, THEME_FOREGROUND));

        current += text_width;

        if (i != path_element_count(_path) - 1)
        {
            Rectangle expand_icon_bound(
                widget_get_bound(this).x() + current,
                widget_get_bound(this).y(),
                _icon_expand->bound(ICON_18PX).width(),
                widget_get_bound(this).height());

            painter.blit_icon(
                *_icon_expand,
                ICON_18PX,
                _icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
                widget_get_color(this, THEME_FOREGROUND));

            current += expand_icon_bound.width();
        }
    }
}
