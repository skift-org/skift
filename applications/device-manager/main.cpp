#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "device-manager/DeviceModel.h"

class DeviceManagerWindow : public Window
{
private:
    Widget *_table;
    DeviceModel *_model;

public:
    DeviceManagerWindow() : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("expansion-card-variant"));
        title("Device Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        _model = device_model_create();

        _table = new Table(root(), (Model *)_model);
        _table->attributes(LAYOUT_FILL);
    }

    ~DeviceManagerWindow() override
    {
        model_destroy((Model *)_model);
    }
};

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new DeviceManagerWindow();
    window->show();

    return application_run();
}
