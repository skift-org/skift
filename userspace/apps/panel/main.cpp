#include <libwidget/Application.h>

#include "panel/windows/Status.h"

namespace Panel
{

struct PanelApplication : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<Status>();
    }
};

} // namespace Panel

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Panel::PanelApplication app;
    return app.run();
}