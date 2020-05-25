
#include <libsystem/filesystem.h>

int mkdir(const char *path)
{
    return filesystem_mkdir(path);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc == 2)
    {
        return mkdir(argv[1]);
    }

    return -1;
}
