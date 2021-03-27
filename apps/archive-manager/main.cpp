#include <libfile/Archive.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>

#include "archive-manager/MainWindow.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto navigation = make<FilePicker::Navigation>();

    MainWindow *window = nullptr;

    if (argc != 2)
    {
        window = new MainWindow(navigation, nullptr);
    }
    else
    {
        auto archive = Archive::open(Path::parse(argv[1]));
        window = new MainWindow(navigation, archive);
    }

    window->show();

    return Widget::Application::run();
}
