
#include <libsystem/filesystem.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        return filesystem_rename(argv[1], argv[2]);
    }

    return -1;
}
