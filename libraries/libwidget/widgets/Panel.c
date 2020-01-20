#include <libwidget/widgets/Panel.h>

Widget *panel_create(Widget *parent)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->destroy = NULL;

    widget_initialize("Panel", WIDGET(panel), parent);

    return WIDGET(panel);
}
