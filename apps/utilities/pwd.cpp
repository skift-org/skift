#include <libio/Streams.h>
#include <libsystem/process/Process.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    char buffer[PATH_LENGTH];
    process_get_directory(buffer, PATH_LENGTH);
    IO::outln("{}", buffer);

    return PROCESS_SUCCESS;
}
