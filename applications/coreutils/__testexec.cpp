
#include <libsystem/Assert.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#define LOOP_MAX 1000

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    process_run("__testexec", NULL);
    return 0;
}
