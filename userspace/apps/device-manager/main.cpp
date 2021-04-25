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
    RefPtr<Widget::Table> _table;

public:
    DeviceManagerWindow() : Widget::Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));

        root()->layout(VFLOW(0));

        root()->add<Widget::TitleBar>(Graphic::Icon::get("expansion-card-variant"), "Device Manager");

        auto model = make<DeviceModel>();

        model->update();

        _table = root()->add<Widget::Table>(model);
        _table->flags(Widget::Element::FILL);
    }
};

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto window = new DeviceManagerWindow();

    window->show();

    return Widget::Application::run();
}
