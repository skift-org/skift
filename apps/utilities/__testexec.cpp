
#include <libsystem/Assert.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>

#define LOOP_MAX 1000

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    Launchpad *launchpad = launchpad_create("__testexec", "/System/Utilities/__testexec");
    return launchpad_launch(launchpad, NULL);
}
