#include <libsystem/eventloop/Timer.h>
#include <libsystem/system/System.h>
#include <libutils/Path.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

#include "device-manager/DeviceModel.h"

class DeviceManagerWindow : public Window
{
private:
    Widget *_table;

public:
    DeviceManagerWindow() : Window(WINDOW_RESIZABLE)
    {
        icon(Icon::get("expansion-card-variant"));
        title("Device Manager");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        auto model = make<DeviceModel>();

        model->update();

        _table = new Table(root(), model);
        _table->attributes(LAYOUT_FILL);
    }
};

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto window = new DeviceManagerWindow();

    window->show();

    return application_run();
}
