#include <libwidget/Application.h>

#include "task-manager/windows/TaskManager.h"

namespace TaskManager
{

struct Application : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<TaskManager>();
    }
};

} // namespace TaskManager

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    TaskManager::Application app;
    return app.run();
}