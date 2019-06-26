#include <skift/application.h>
#include <skift/window.h>

int main(int argc, char **argv)
{
    application_init(argc, argv, "Test Application");
    
    window_t* mainwin = window();

    return application_run();
}
