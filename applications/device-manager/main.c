#include <libsystem/eventloop/Timer.h>
#include <libsystem/system.h>
#include <libwidget/Application.h>
#include <libwidget/Container.h>
#include <libwidget/Graph.h>
#include <libwidget/Icon.h>
#include <libwidget/Label.h>
#include <libwidget/Panel.h>
#include <libwidget/Table.h>

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
    window_initialize((Window *)window, "expansion-card-variant", "Device Manager", 700, 500, WINDOW_RESIZABLE);
    window_root((Window *)window)->layout = VFLOW(0);

    /// --- Table view --- //
    window->table_model = device_model_create();

    window->table = table_create(window_root((Window *)window), (Model *)window->table_model);
    window->table->layout_attributes = LAYOUT_FILL;

    window_show((Window *)window);

    return application_run();
}
