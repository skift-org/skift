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
    icon_create(navbar, "/res/icon/home.png");

    separator_create(navbar);

    window->Breadcrumb = breadcrumb_create(navbar, current_path);
    WIDGET(window->Breadcrumb)->layout_attributes = LAYOUT_FILL;

    separator_create(navbar);

    icon_create(navbar, "/res/icon/refresh.png");

    /// --- Table view --- ///
    window->model = filesystem_model_create(current_path);
    Widget *table = table_create(root, (Model *)window->model);
    table->layout_attributes = LAYOUT_FILL;
    table->insets = INSETS(8, 8);

    return window;
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Window *window = (Window *)file_explorer_window_create("/bin");
    window_show(window);

    return application_run();
}
