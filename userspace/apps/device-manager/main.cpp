#include <libio/Path.h>

#include <libasync/Timer.h>
#include <libsystem/system/System.h>

#include <libwidget/Application.h>
#include <libwidget/Table.h>
#include <libwidget/TitleBar.h>

#include "device-manager/DeviceModel.h"

class DeviceManagerWindow : public Widget::Window
{
private:
    Widget::Component *_table;

public:
    DeviceManagerWindow() : Widget::Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));

        root()->layout(VFLOW(0));

        new Widget::TitleBar(
            root(),
            Graphic::Icon::get("expansion-card-variant"),
            "Device Manager");

        auto model = make<DeviceModel>();

        model->update();

        _table = new Widget::Table(root(), model);
        _table->flags(Widget::Component::FILL);
    }
};

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto window = new DeviceManagerWindow();

    window->show();

    return Widget::Application::run();
}
