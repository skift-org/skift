#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    char buffer[PATH_LENGTH];
    process_get_directory(buffer, PATH_LENGTH);
    printf("%s", buffer);

    return PROCESS_SUCCESS;
}
