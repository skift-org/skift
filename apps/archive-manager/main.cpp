#include <libfile/ZipArchive.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>

#include "archive-manager/MainWindow.h"

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    if (argc != 2)
    {
        return PROCESS_FAILURE;
    }

    auto navigation = make<filepicker::Navigation>();
    auto archive = make<ZipArchive>(argv[1]);

    auto window = new MainWindow(navigation, archive);

    window->show();

    return Application::run();
}
