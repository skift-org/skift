#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Label.h>

void label_paint(Label *label, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter_draw_string_within(
        painter,
        widget_font(),
        label->text,
        widget_get_bound(label),
        POSITION_CENTER,
        widget_get_color(label, THEME_FOREGROUND));
}

Vec2i label_size(Label *label)
{
    return vec2i(font_measure_string(widget_font(), label->text), 16);
}

void label_set_text(Widget *label, const char *text)
{
    free(((Label *)label)->text);
    ((Label *)label)->text = strdup(text);

    widget_update(label);
}

void label_destroy(Label *label)
{
    free(label->text);
}

static const WidgetClass label_class = {
    .name = "Label",

    .destroy = (WidgetDestroyCallback)label_destroy,
    .paint = (WidgetPaintCallback)label_paint,
    .size = (WidgetComputeSizeCallback)label_size,
};

Widget *label_create(Widget *parent, const char *text)
{
    Label *label = __create(Label);

    label->text = strdup(text);

    widget_initialize(WIDGET(label), &label_class, parent);

    return WIDGET(label);
}
