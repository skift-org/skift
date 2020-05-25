
#include <libsystem/filesystem.h>

int unlink(const char *path)
{
    return filesystem_unlink(path);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return unlink(argv[1]);
    }

    return -1;
}
