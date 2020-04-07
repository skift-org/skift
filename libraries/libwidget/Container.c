#include <libwidget/Container.h>

Widget *container_create(Widget *parent)
{
    Container *container = __create(Container);

    WIDGET(container)->destroy = NULL;

    widget_initialize(WIDGET(container), "Container", parent);

    return WIDGET(container);
}
