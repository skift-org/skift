#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Icon.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>
#include <libwidget/Table.h>

#include "file-explorer/Breadcrumb.h"
#include "file-explorer/FileSystemModel.h"

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

    Window *window = window_create("/res/icon/folder.png", "File Explorer", 500, 400, WINDOW_RESIZABLE);

    window_root(window)->layout = (Layout){LAYOUT_VFLOW, 0, 0};

    file_manager_create_navbar(window_root(window));
    Widget *table = table_create(window_root(window), filesystem_model_create());
    table->layout_attributes = LAYOUT_FILL;
    table->insets = INSETS(8, 8);

    window_show(window);

    return application_run();
}
