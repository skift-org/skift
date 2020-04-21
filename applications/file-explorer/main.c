#include <libsystem/cstring.h>
#include <libsystem/logger.h>
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
    Path *current_path;
} FileExplorerWindow;

void file_explorer_window_navigate(FileExplorerWindow *window, Path *path)
{
    ((Table *)window->table)->selected = -1;

    if (window->current_path)
    {
        path_destroy(window->current_path);
    }

    window->current_path = path;

    breadcrumb_navigate(window->Breadcrumb, path);
    filesystem_model_navigate(window->model, path);
    widget_update(window->table);
}

void file_explorer_go_home(FileExplorerWindow *window, ...)
{
    file_explorer_window_navigate(window, path_create("/"));
}

void file_explorer_go_up(FileExplorerWindow *window, ...)
{
    Path *new_path = path_clone(window->current_path);
    char *poped_element = path_pop(new_path);
    if (poped_element)
    {
        free(poped_element);
        file_explorer_window_navigate(window, new_path);
    }
}

void file_explorer_table_open(FileExplorerWindow *window, ...)
{
    if (((Table *)window->table)->selected >= 0)
    {
        Path *new_path = path_clone(window->current_path);
        path_push(new_path, strdup(filesystem_model_filename_by_index(window->model, ((Table *)window->table)->selected)));
        file_explorer_window_navigate(window, new_path);
    }
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

    window->current_path = path_create(current_path);

    /// --- Navigation bar --- ///
    Widget *navbar = panel_create(root);

    navbar->layout = (Layout){LAYOUT_HFLOW, 8, 0};
    navbar->insets = INSETS(0, 8);

    icon_create(navbar, "/res/icon/arrow_backward.png");
    icon_create(navbar, "/res/icon/arrow_forward.png");

    Widget *up_button = icon_create(navbar, "/res/icon/arrow_upward.png");
    widget_set_event_handler(up_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)file_explorer_go_up);

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

    widget_set_event_handler(window->table, EVENT_MOUSE_DOUBLE_CLICK, window, (WidgetEventHandlerCallback)file_explorer_table_open);

    return window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = (Window *)file_explorer_window_create("/");
    window_show(window);

    return application_run();
}
