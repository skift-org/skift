#include <libform/application.h>
#include <libform/form.h>

int main(int argc, char **argv)
{
    application_init(argc, argv, "Test Application");

    return application_run();
}
