#include <libio/Path.h>

#include <libasync/Timer.h>
#include <libsystem/system/System.h>

#include <libwidget/Application.h>
#include <libwidget/Components.h>
#include <libwidget/Views.h>

#include "device-manager/DeviceModel.h"

struct DeviceManagerWindow : public Widget::Window
{
private:
    RefPtr<DeviceModel> _model;

public:
    DeviceManagerWindow() : Widget::Window(WINDOW_RESIZABLE)
    {
        size(Math::Vec2i(700, 500));
        _model = make<DeviceModel>();
        _model->update();
    }

    RefPtr<Widget::Element> build() override
    {
        using namespace Widget;

        return vflow({
            titlebar(Graphic::Icon::get("expansion-card-variant"), "Device Manager"),
            fill(table(_model)),
        });
    }
};

int main(int, char **)
{
    auto window = new DeviceManagerWindow();

    window->show();

    return Widget::Application::the().run();
}
