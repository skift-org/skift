#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/Icon.h>
#include <libwidget/widgets/Label.h>
#include <libwidget/widgets/Panel.h>
#include <libwidget/widgets/Toolbar.h>

Widget *toolbar_create(Widget *parent)
{
    Widget *toolbar = panel_create(parent);

    toolbar->layout = HFLOW(4);
    toolbar->insets = INSETS(4, 4);
    toolbar->max_height = 38;
    toolbar->min_height = 38;

    return toolbar;
}

Widget *toolbar_icon_create(Widget *parent, const char *icon)
{
    Widget *toolbar_icon = button_create(parent);
    toolbar_icon->layout = STACK();
    toolbar_icon->insets = INSETS(6);
    icon_create(toolbar_icon, icon);
    return toolbar_icon;
}

Widget *toolbar_icon_with_text_create(Widget *parent, const char *icon, const char *text)
{
    Widget *toolbar_icon = button_create(parent);

    toolbar_icon->layout = HFLOW(4);
    toolbar_icon->insets = INSETS(0, 0, 4, 8);

    icon_create(toolbar_icon, icon);
    label_create(toolbar_icon, text);

    return toolbar_icon;
}
