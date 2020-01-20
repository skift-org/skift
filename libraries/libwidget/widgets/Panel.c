#include <libwidget/widgets/Panel.h>

Widget *panel_create(Widget *parent, Rectangle bound)
{
    Panel *panel = __create(Panel);

    WIDGET(panel)->destroy = NULL;

    widget_initialize(WIDGET(panel), "Panel", parent, bound);

    return WIDGET(panel);
}
