
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>

Result mkdir(const char *path)
{
    if(directory_exist(path)){
        printf("mkdir: cannot create directory '%s': File exists", path);
        return ERR_FILE_EXISTS;
    }

    return filesystem_mkdir(path);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return mkdir(argv[1]);
    }

    return -1;
}
