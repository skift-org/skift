
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Directory.h>

struct Directory
{
    Handle handle;
};

Directory *directory_open(const char *path, OpenFlag flags)
{
    Directory *directory = __create(Directory);

    __plug_handle_open(HANDLE(directory), path, flags | OPEN_DIRECTORY);

    return directory;
}

void directory_close(Directory *directory)
{
    __plug_handle_close(HANDLE(directory));

    free(directory);
}

int directory_read(Directory *directory, DirectoryEntry *entry)
{
    return __plug_handle_read(HANDLE(directory), entry, sizeof(DirectoryEntry));
}

bool directory_exist(const char *path)
{
    Directory *directory = directory_open(path, OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return false;
    }

    directory_close(directory);
    return true;
}
