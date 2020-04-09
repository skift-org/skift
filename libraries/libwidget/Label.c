#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>

#include <libwidget/Label.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

void label_paint(Label *label, Painter *painter)
{
    int text_width = painter_mesure_string(painter, widget_font(), label->text);

    painter_draw_string(
        painter,
        widget_font(),
        label->text,
        (Point){
            widget_bound(label).X + widget_bound(label).width / 2 - text_width / 2,
            widget_bound(label).Y + widget_bound(label).height / 2 + 4,
        },
        THEME_FOREGROUND);
}

void label_destroy(Label *label)
{
    free(label->text);
}

Widget *label_create(Widget *parent, const char *text)
{
    Label *label = __create(Label);

    label->text = strdup(text);

    WIDGET(label)->paint = (WidgetPaintCallback)label_paint;
    WIDGET(label)->destroy = (WidgetDestroyCallback)label_destroy;

    widget_initialize(WIDGET(label), "Label", parent);

    return WIDGET(label);
}
