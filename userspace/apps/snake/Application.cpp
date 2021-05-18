#include <libwidget/Application.h>

#include "snake/windows/Game.h"

namespace Snake
{

struct Application : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<Game>();
    }
};

} // namespace Snake

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Snake::Application app;
    return app.run();
}
