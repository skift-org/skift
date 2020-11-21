#include <libsystem/process/Process.h>
#include <libwidget/Application.h>

#include "file-manager/windows/Main.h"

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    auto navigation = make<file_manager::Navigation>();
    auto bookmarks = file_manager::Bookmarks::load();

    auto window = new file_manager::Main(navigation, bookmarks);

    navigation->navigate(Path::parse("/User"), file_manager::Navigation::NONE);
    window->show();

    return application_run();
}
