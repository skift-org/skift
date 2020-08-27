#include <libwidget/Application.h>
#include <libwidget/Markup.h>

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        application_initialize(argc, argv);

        Window *window = window_create_from_file(argv[1]);

        window->show();

        return application_run();
    }
    else
    {
        return -1;
    }
}
