#include <libsystem/io/Stream.h>
#include <libsystem/process/Environment.h>

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);
    printf("%s", environment_copy().cstring());
    return PROCESS_SUCCESS;
}
