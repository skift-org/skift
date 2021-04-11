#include <libsystem/process/Process.h>
#include <libwidget/Application.h>

#include "file-manager/MainWindow.h"

int main(int argc, char **argv)
{
    Widget::Application::initialize(argc, argv);

    auto navigation = make<FilePicker::Navigation>();
    auto bookmarks = FilePicker::Bookmarks::load();

    auto window = new MainWindow(navigation, bookmarks);

    navigation->go_home_dont_record_history();
    window->show();

    return Widget::Application::run();
}
