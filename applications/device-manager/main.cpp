#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "device-manager/DeviceModel.h"

typedef struct
{
    Window window;

    /// --- Table view --- //
    Widget *table;
    DeviceModel *table_model;
} TaskManagerWindow;

void widget_ram_update(Graph *widget)
{
    SystemStatus status = system_get_status();

    graph_record(widget, status.used_ram / (double)status.total_ram);
}

void widget_cpu_update(Graph *widget)
{
    SystemStatus status = system_get_status();

    graph_record(widget, status.cpu_usage / 100.0);
}

void widget_table_update(TaskManagerWindow *window)
{
    model_update((Model *)window->table_model);
    widget_update(window->table);
    widget_layout(window->table);
}

void task_manager_window_destroy(TaskManagerWindow *window)
{
    model_destroy((Model *)window->table_model);
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    TaskManagerWindow *window = __create(TaskManagerWindow);

    window_initialize((Window *)window, WINDOW_RESIZABLE);

    window_set_icon((Window *)window, icon_get("expansion-card-variant"));
    window_set_title((Window *)window, "Device Manager");
    window_set_size((Window *)window, Vec2i(700, 500));

    window_root((Window *)window)->layout = VFLOW(0);

    /// --- Table view --- //
    window->table_model = device_model_create();

    window->table = table_create(window_root((Window *)window), (Model *)window->table_model);
    window->table->layout_attributes = LAYOUT_FILL;

    window_show((Window *)window);

    return application_run();
}
