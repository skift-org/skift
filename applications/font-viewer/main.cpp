#include <libtruetype/File.h>
#include <libwidget/Application.h>

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    truetype::File file{"System/Fonts/Roboto/Roboto-Regular.ttf"};

    return application_run();
}
