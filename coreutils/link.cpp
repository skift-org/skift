
#include <libsystem/Result.h>
#include <libraries/libsystem/io/Filesystem.h>

int link(const char *src, const char *dest)
{
    return filesystem_link(src, dest);
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        return link(argv[1], argv[2]);
    }

    return -1;
}
