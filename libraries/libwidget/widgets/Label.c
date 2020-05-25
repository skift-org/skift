#include <libgraphic/Painter.h>
#include <libsystem/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Label.h>

void label_paint(Label *label, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);
    int text_width = font_measure_string(widget_font(), label->text);

    painter_draw_string(
        painter,
        widget_font(),
        label->text,
        vec2i(
            widget_get_bound(label).x + widget_get_bound(label).width / 2 - text_width / 2,
            widget_get_bound(label).y + widget_get_bound(label).height / 2 + 4),
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

    .paint = (WidgetPaintCallback)label_paint,
    .destroy = (WidgetDestroyCallback)label_destroy,
    .size = (WidgetComputeSizeCallback)label_size,
};

Widget *label_create(Widget *parent, const char *text)
{
    Label *label = __create(Label);

    label->text = strdup(text);

    widget_initialize(WIDGET(label), &label_class, parent);

    return WIDGET(label);
}
