#include <libwidget/Application.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Icon.h>
#include <libwidget/Panel.h>
#include <libwidget/Separator.h>
#include <libwidget/Table.h>

#include "file-explorer/Breadcrumb.h"
#include "file-explorer/FileSystemModel.h"

typedef struct
{
    Window window;

    Widget *Breadcrumb;
    Widget *table;
    FileSystemModel *model;
} FileExplorerWindow;

void file_explorer_window_navigate(FileExplorerWindow *window, const char *path)
{
    breadcrumb_navigate(window->Breadcrumb, path);
    filesystem_model_navigate(window->model, path);
    widget_update(window->table);
}

void file_explorer_go_home(FileExplorerWindow *window, ...)
{
    file_explorer_window_navigate(window, "/home");
}

void file_explorer_window_destroy(FileExplorerWindow *window)
{
    model_destroy((Model *)window->model);
}

FileExplorerWindow *file_explorer_window_create(const char *current_path)
{
    FileExplorerWindow *window = __create(FileExplorerWindow);

    window_initialize((Window *)window, "/res/icon/folder.png", "File Explorer", 500, 400, WINDOW_RESIZABLE);
    Widget *root = window_root((Window *)window);
    root->layout = (Layout){LAYOUT_VFLOW, 0, 0};

    /// --- Navigation bar --- ///
    Widget *navbar = panel_create(root);

    navbar->layout = (Layout){LAYOUT_HFLOW, 8, 0};
    navbar->insets = INSETS(0, 8);

    icon_create(navbar, "/res/icon/arrow_backward.png");
    icon_create(navbar, "/res/icon/arrow_forward.png");
    icon_create(navbar, "/res/icon/arrow_upward.png");

    Widget *home_button = icon_create(navbar, "/res/icon/home.png");
    widget_set_event_handler(home_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)file_explorer_go_home);

    separator_create(navbar);

    window->Breadcrumb = breadcrumb_create(navbar, current_path);
    WIDGET(window->Breadcrumb)->layout_attributes = LAYOUT_FILL;

    separator_create(navbar);

    icon_create(navbar, "/res/icon/refresh.png");

    /// --- Table view --- ///
    window->model = filesystem_model_create(current_path);
    window->table = table_create(root, (Model *)window->model);
    window->table->layout_attributes = LAYOUT_FILL;
    window->table->insets = INSETS(8, 8);

    return window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = (Window *)file_explorer_window_create("/");
    window_show(window);

    return application_run();
}
