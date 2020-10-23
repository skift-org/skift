#include <libgraphic/Painter.h>
#include <libwidget/Window.h>

#include "file-manager/Breadcrumb.h"

Breadcrumb::Breadcrumb(Widget *parent, Path path)
    : Widget(parent), _path(path)
{
    _path = path;
    _icon_computer = Icon::get("laptop");
    _icon_expand = Icon::get("chevron-right");
}

void Breadcrumb::navigate(Path path)
{
    _path = path;
    should_repaint();
}

void Breadcrumb::paint(Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    int current = 0;

    Rectangle computer_icon_bound(
        bound().x(),
        bound().y(),
        _icon_computer->bound(ICON_18PX).width() + 16,
        bound().height());

    painter.blit_icon(
        *_icon_computer,
        ICON_18PX,
        _icon_computer->bound(ICON_18PX).centered_within(computer_icon_bound),
        color(THEME_FOREGROUND));

    current += computer_icon_bound.width();

    if (_path.length() > 0)
    {
        Rectangle expand_icon_bound(
            bound().x() + current,
            bound().y(),
            _icon_expand->bound(ICON_18PX).width(),
            bound().height());

        painter.blit_icon(
            *_icon_expand,
            ICON_18PX,
            _icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
            color(THEME_FOREGROUND));

        current += expand_icon_bound.width();
    }

    for (size_t i = 0; i < _path.length(); i++)
    {
        auto element = _path[i];

        int text_width = font()->mesure_string(element.cstring()).width();

        Rectangle element_bound(
            bound().x() + current,
            bound().y(),
            text_width,
            bound().height());

        painter.draw_string(
            *font(),
            element.cstring(),
            element_bound.position() + Vec2i(0, 19),
            color(THEME_FOREGROUND));

        current += text_width;

        if (i != _path.length() - 1)
        {
            Rectangle expand_icon_bound(
                bound().x() + current,
                bound().y(),
                _icon_expand->bound(ICON_18PX).width(),
                bound().height());

            painter.blit_icon(
                *_icon_expand,
                ICON_18PX,
                _icon_expand->bound(ICON_18PX).centered_within(expand_icon_bound),
                color(THEME_FOREGROUND));

            current += expand_icon_bound.width();
        }
    }
}
