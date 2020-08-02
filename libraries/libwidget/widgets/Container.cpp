#include <libwidget/widgets/Container.h>

static const WidgetClass container_class = {
    .name = "Container",
};

Widget *container_create(Widget *parent)
{
    Container *container = __create(Container);

    widget_initialize(container, &container_class, parent);

    return container;
}
