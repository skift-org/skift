#include <libwidget/Container.h>

Widget *container_create(Widget *parent, Rectangle bound)
{
    Container *container = __create(Container);

    WIDGET(container)->destroy = NULL;

    widget_initialize(WIDGET(container), "Container", parent, bound);

    return WIDGET(container);
}
