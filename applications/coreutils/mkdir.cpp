
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Filesystem.h>
#include <stdio.h>
#include <unistd.h>

int mkdir(const char *path)
{
    return filesystem_mkdir(path);
}

void mkdir_parent_dirs(const char *path)
{
    if (directory_exist(path))
        return;

    const char *iter = path;
    size_t path_len = strlen(path);
    char *construct_parent_dirs = (char *)calloc(path_len + 1, sizeof(char));
    char *iter_recursively = construct_parent_dirs;

    while (*iter != '\0')
    {
        *iter_recursively = *iter;

        if (*iter == '/')
            if (!directory_exist(construct_parent_dirs))
                filesystem_mkdir(construct_parent_dirs);

        ++iter_recursively;
        ++iter;
    }

    --iter;
    if (*iter != '/')
        if (!directory_exist(construct_parent_dirs))
            filesystem_mkdir(construct_parent_dirs);

    free(construct_parent_dirs);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return mkdir(argv[1]);
    }

    if (argc == 3)
    {
        if (!strcmp(argv[1], "-p") || !strcmp(argv[1], "--parents"))
            mkdir_parent_dirs(argv[2]);

        return 0;
    }
    return -1;
}
