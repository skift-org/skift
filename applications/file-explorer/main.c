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

typedef enum
{
    RECORD_BACKWARD,
    RECORD_FOREWARD,
} RecordHistory;

typedef struct
{
    Window window;

    /// --- Navigation bar --- ///
    Widget *go_backward;
    Widget *go_foreward;
    Widget *go_up;
    Widget *go_home;
    Widget *breadcrumb;

    /// --- Table view --- ///
    Widget *table;
    FileSystemModel *model;

    /// --- Navigation --- //
    List *backward_history;
    List *foreward_history;
    Path *current_path;
} FileExplorerWindow;

static void update_navigation_bar(FileExplorerWindow *window)
{
    widget_set_enable(window->go_backward, list_any(window->backward_history));
    widget_set_enable(window->go_foreward, list_any(window->foreward_history));
    widget_set_enable(window->go_up, path_element_count(window->current_path) > 0);
}

static void navigate(FileExplorerWindow *window, Path *path, RecordHistory record_history)
{
    ((Table *)window->table)->selected = -1;

    if (record_history == RECORD_BACKWARD)
    {
        if (window->current_path)
        {
            list_pushback(window->backward_history, window->current_path);
        }
    }
    else if (record_history == RECORD_FOREWARD)
    {
        if (window->current_path)
        {
            list_pushback(window->foreward_history, window->current_path);
        }
    }

    window->current_path = path;

    breadcrumb_navigate(window->breadcrumb, path);
    filesystem_model_navigate(window->model, path);
    widget_update(window->table);
    update_navigation_bar(window);
}

static void clear_foreward_history(FileExplorerWindow *window)
{
    list_clear_with_callback(window->foreward_history, (ListDestroyElementCallback)path_destroy);
}

static void open(FileExplorerWindow *window, ...)
{
    if (((Table *)window->table)->selected >= 0)
    {
        Path *new_path = path_clone(window->current_path);
        path_push(new_path, strdup(filesystem_model_filename_by_index(window->model, ((Table *)window->table)->selected)));

        clear_foreward_history(window);
        navigate(window, new_path, RECORD_BACKWARD);
    }
}

static void go_home(FileExplorerWindow *window, ...)
{
    clear_foreward_history(window);
    navigate(window, path_create("/"), RECORD_BACKWARD);
}

static void go_up(FileExplorerWindow *window, ...)
{
    Path *new_path = path_clone(window->current_path);
    char *poped_element = path_pop(new_path);

    if (poped_element)
    {
        free(poped_element);

        clear_foreward_history(window);
        navigate(window, new_path, RECORD_BACKWARD);
    }
}

static void go_backward(FileExplorerWindow *window, ...)
{
    if (list_any(window->backward_history))
    {
        Path *path = NULL;
        list_popback(window->backward_history, (void **)&path);

        navigate(window, path, RECORD_FOREWARD);
    }
}

static void go_foreward(FileExplorerWindow *window, ...)
{
    if (list_any(window->foreward_history))
    {
        Path *path = NULL;
        list_popback(window->foreward_history, (void **)&path);

        navigate(window, path, RECORD_BACKWARD);
    }
}

void file_explorer_window_destroy(FileExplorerWindow *window)
{
    model_destroy((Model *)window->model);
}

Window *file_explorer_window_create(const char *current_path)
{
    FileExplorerWindow *window = __create(FileExplorerWindow);

    window_initialize((Window *)window, "folder", "File Explorer", 500, 400, WINDOW_RESIZABLE);
    Widget *root = window_root((Window *)window);
    root->layout = VFLOW(0);

    window->current_path = path_create(current_path);
    window->backward_history = list_create();
    window->foreward_history = list_create();

    /// --- Navigation bar --- ///
    Widget *navbar = panel_create(root);

    navbar->layout = HFLOW(12);
    navbar->insets = INSETS(0, 8);

    Widget *backward_button = icon_create(navbar, "arrow-left");
    widget_set_event_handler(backward_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)go_backward);
    window->go_backward = backward_button;

    Widget *foreward_button = icon_create(navbar, "arrow-right");
    widget_set_event_handler(foreward_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)go_foreward);
    window->go_foreward = foreward_button;

    Widget *up_button = icon_create(navbar, "arrow-up");
    widget_set_event_handler(up_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)go_up);
    window->go_up = up_button;

    Widget *home_button = icon_create(navbar, "home");
    widget_set_event_handler(home_button, EVENT_MOUSE_BUTTON_PRESS, window, (WidgetEventHandlerCallback)go_home);
    window->go_home = home_button;

    separator_create(navbar);

    window->breadcrumb = breadcrumb_create(navbar, current_path);
    WIDGET(window->breadcrumb)->layout_attributes = LAYOUT_FILL;

    separator_create(navbar);

    icon_create(navbar, "refresh");

    update_navigation_bar(window);

    /// --- Table view --- ///
    window->model = filesystem_model_create(current_path);
    window->table = table_create(root, (Model *)window->model);
    window->table->layout_attributes = LAYOUT_FILL;

    widget_set_event_handler(window->table, EVENT_MOUSE_DOUBLE_CLICK, window, (WidgetEventHandlerCallback)open);

    return (Window *)window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = file_explorer_window_create("/");
    window_show(window);

    return application_run();
}
