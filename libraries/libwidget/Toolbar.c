#include <libwidget/Button.h>
#include <libwidget/Icon.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/Toolbar.h>

Widget *toolbar_create(Widget *parent)
{
    Widget *toolbar = panel_create(parent);

    toolbar->layout = HFLOW(0);
    toolbar->insets = INSETS(0, 4);
    toolbar->max_height = 32;
    toolbar->min_height = 32;

    return toolbar;
}

Widget *toolbar_icon_create(Widget *parent, const char *icon)
{
    Widget *toolbar_icon = button_create(parent, "");
    icon_create(toolbar_icon, icon);
    return toolbar_icon;
}

Widget *toolbar_icon_with_text_create(Widget *parent, const char *icon, const char *text)
{
    Widget *toolbar_icon = button_create(parent, "");

    toolbar_icon->layout = HFLOW(4);
    toolbar_icon->insets = INSETS(0, 4);

    icon_create(toolbar_icon, icon);
    label_create(toolbar_icon, text);

    return toolbar_icon;
}
