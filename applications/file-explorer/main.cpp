#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "file-explorer/Breadcrumb.h"
#include "file-explorer/FileSystemModel.h"

enum RecordHistory
{
    RECORD_BACKWARD,
    RECORD_FOREWARD,
};

struct FileExplorerWindow
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
    Path *current_path;
    List *foreward_history;
};

static void update_navigation_bar(FileExplorerWindow *window)
{
    widget_set_enable(window->go_backward, list_any(window->backward_history));
    widget_set_enable(window->go_foreward, list_any(window->foreward_history));
    widget_set_enable(window->go_up, path_element_count(window->current_path) > 0);
}

static void navigate(FileExplorerWindow *window, Path *path, RecordHistory record_history)
{
    if (path_equals(window->current_path, path))
    {
        path_destroy(path);
        return;
    }

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
    widget_layout(window->table);
    update_navigation_bar(window);
}

static void clear_foreward_history(FileExplorerWindow *window)
{
    list_clear_with_callback(window->foreward_history, (ListDestroyElementCallback)path_destroy);
}

static void on_open(FileExplorerWindow *window, ...)
{
    if (((Table *)window->table)->selected >= 0)
    {
        if (filesystem_model_filetype_by_index(window->model, ((Table *)window->table)->selected) == FILE_TYPE_DIRECTORY)
        {
            Path *new_path = path_clone(window->current_path);
            path_push(new_path, strdup(filesystem_model_filename_by_index(window->model, ((Table *)window->table)->selected)));
            clear_foreward_history(window);
            navigate(window, new_path, RECORD_BACKWARD);
        }
        else
        {
            Launchpad *launchpad = launchpad_create("open", "/System/Binaries/open");
            launchpad_argument(launchpad, filesystem_model_filename_by_index(window->model, ((Table *)window->table)->selected));
            launchpad_launch(launchpad, nullptr);
        }
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
        Path *path = nullptr;
        list_popback(window->backward_history, (void **)&path);

        navigate(window, path, RECORD_FOREWARD);
    }
}

static void go_foreward(FileExplorerWindow *window, ...)
{
    if (list_any(window->foreward_history))
    {
        Path *path = nullptr;
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

    window_initialize((Window *)window, WINDOW_RESIZABLE);

    window_set_icon((Window *)window, Icon::get("folder"));
    window_set_title((Window *)window, "File Explorer");
    window_set_size((Window *)window, Vec2i(700, 500));

    Widget *root = window_root((Window *)window);
    root->layout = VFLOW(0);

    window->current_path = path_create(current_path);
    window->backward_history = list_create();
    window->foreward_history = list_create();

    /// --- Navigation bar --- ///
    Widget *toolbar = toolbar_create(root);

    Widget *backward_button = toolbar_icon_create(toolbar, Icon::get("arrow-left"));
    widget_set_event_handler(backward_button, EVENT_ACTION, EVENT_HANDLER(window, (EventHandlerCallback)go_backward));
    window->go_backward = backward_button;

    Widget *foreward_button = toolbar_icon_create(toolbar, Icon::get("arrow-right"));
    widget_set_event_handler(foreward_button, EVENT_ACTION, EVENT_HANDLER(window, (EventHandlerCallback)go_foreward));
    window->go_foreward = foreward_button;

    Widget *up_button = toolbar_icon_create(toolbar, Icon::get("arrow-up"));
    widget_set_event_handler(up_button, EVENT_ACTION, EVENT_HANDLER(window, (EventHandlerCallback)go_up));
    window->go_up = up_button;

    Widget *home_button = toolbar_icon_create(toolbar, Icon::get("home"));
    widget_set_event_handler(home_button, EVENT_ACTION, EVENT_HANDLER(window, (EventHandlerCallback)go_home));
    window->go_home = home_button;

    separator_create(toolbar);

    window->breadcrumb = breadcrumb_create(toolbar, current_path);
    WIDGET(window->breadcrumb)->layout_attributes = LAYOUT_FILL;

    separator_create(toolbar);

    toolbar_icon_create(toolbar, Icon::get("refresh"));

    update_navigation_bar(window);

    /// --- Table view --- ///
    window->model = filesystem_model_create(current_path);
    window->table = table_create(root, (Model *)window->model);
    window->table->layout_attributes = LAYOUT_FILL;

    widget_set_event_handler(window->table, EVENT_MOUSE_DOUBLE_CLICK, EVENT_HANDLER(window, (EventHandlerCallback)on_open));

    return (Window *)window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = file_explorer_window_create("/");
    window_show(window);

    return application_run();
}
