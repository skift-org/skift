#include <libwidget/widgets/Container.h>

static const WidgetClass container_class = {};

Container *container_create(Widget *parent)
{
    auto container = __create(Container);

    widget_initialize(container, &container_class, parent);

    return container;
}
