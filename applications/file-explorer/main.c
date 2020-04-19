#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Icon.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>

#include "file-explorer/Breadcrumb.h"

void file_manager_create_navbar(Widget *parent)
{
    Widget *navbar = panel_create(parent);

    navbar->layout = (Layout){LAYOUT_HFLOW, 8, 0};
    navbar->insets = INSETS(0, 8);

    icon_create(navbar, "/res/icon/arrow_backward.png");
    icon_create(navbar, "/res/icon/arrow_forward.png");
    icon_create(navbar, "/res/icon/arrow_upward.png");

    separator_create(navbar);

    Widget *breadcrumb = breadcrumb_create(navbar);
    WIDGET(breadcrumb)->layout_attributes = LAYOUT_FILL;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = window_create("/res/icon/folder.png", "File Explorer", 500, 400, WINDOW_NONE);

    window_root(window)->layout = (Layout){LAYOUT_VFLOW, 0, 8};

    file_manager_create_navbar(window_root(window));

    window_show(window);

    return application_run();
}
