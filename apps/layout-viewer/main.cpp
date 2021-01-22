#include <libwidget/Application.h>
#include <libwidget/Markup.h>

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        Application::initialize(argc, argv);

        Window *window = window_create_from_file(argv[1]);

        window->show();

        return Application::run();
    }
    else
    {
        return PROCESS_FAILURE;
    }
}
