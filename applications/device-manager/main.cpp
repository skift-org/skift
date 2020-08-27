#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "device-manager/DeviceModel.h"

struct TaskManagerWindow : public Window
{
    /// --- Table view --- //
    Widget *table;
    DeviceModel *table_model;
};

void task_manager_window_destroy(TaskManagerWindow *window)
{
    model_destroy((Model *)window->table_model);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    TaskManagerWindow *window = __create(TaskManagerWindow);

    window_initialize(window, WINDOW_RESIZABLE);

    window->icon(Icon::get("expansion-card-variant"));
    window->title("Device Manager");
    window->size(Vec2i(700, 500));

    window->root()->layout(VFLOW(0));

    /// --- Table view --- //
    window->table_model = device_model_create();

    window->table = new Table(window->root(), (Model *)window->table_model);
    window->table->attributes(LAYOUT_FILL);

    window->show();

    return application_run();
}
