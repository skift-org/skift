#include <libwidget/widgets/Button.h>
#include <libwidget/widgets/IconPanel.h>
#include <libwidget/widgets/Label.h>
#include <libwidget/widgets/Panel.h>
#include <libwidget/widgets/Toolbar.h>

Widget *toolbar_create(Widget *parent)
{
    auto toolbar = new Panel(parent);

    toolbar->layout(HFLOW(4));
    toolbar->insets(Insets(4, 4));
    toolbar->max_height(38);
    toolbar->min_height(38);

    return toolbar;
}

Widget *toolbar_icon_create(Widget *parent, RefPtr<Icon> icon)
{
    auto toolbar_icon = new Button(parent, BUTTON_TEXT);
    toolbar_icon->layout(STACK());
    toolbar_icon->insets(Insets(6));

    new IconPanel(toolbar_icon, icon);

    return toolbar_icon;
}

Widget *toolbar_icon_with_text_create(Widget *parent, RefPtr<Icon> icon, const char *text)
{
    auto toolbar_icon = new Button(parent, BUTTON_OUTLINE);

    toolbar_icon->layout(HFLOW(4));
    toolbar_icon->insets(Insets(0, 0, 4, 8));

    new IconPanel(toolbar_icon, icon);
    new Label(toolbar_icon, text);

    return toolbar_icon;
}
