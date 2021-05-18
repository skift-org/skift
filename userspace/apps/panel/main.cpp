#include <libwidget/Application.h>

#include "panel/windows/Status.h"

namespace Panel
{

struct Application : public Widget::Application
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

    Panel::Application app;
    return app.run();
}