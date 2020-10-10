#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>

Result rmdir(const char *path)
{
    if(!directory_exist(path))
    {
        printf("rmdir: failed to remove '%s': No such file or directory", path);
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    Result result = filesystem_rmdir(path);
    
    if (result == ERR_NOT_READABLE)
    {
        printf("rmdir: failed to remove '%s': Not readable", path);
    }
    else if(result == ERR_DIRECTORY_NOT_EMPTY)
    {
        printf("rmdir: failed to remove '%s': Directory not empty", path);
    }
    
    return result;
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return rmdir(argv[1]);
    }
}