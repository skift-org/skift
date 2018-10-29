#include <string.h>
#include <skift/path.h>

int path_read(const char *path, int index, char *buffer)
{
    int current_index = 0;
    buffer[0] = '\0';

    if (path[0] == '/')
        path++;

    for (int i = 0; path[i]; i++)
    {
        char c = path[i];

        if (current_index == index)
        {
            if (c == '/')
            {
                break;
            }
            else
            {
                strapd(buffer, c);
            }
        }

        if (c == '/')
        {
            current_index++;
            buffer[0] = '\0';
        }
    }

    return (current_index == index && strlen(buffer)) ? 1 : 0;
}

int path_len(const char *path)
{
    char buffer[128];
    int count;

    for (count = 0; path_read(path, count, buffer); count++)
    {
    }

    return count;
}

int path_split(const char *path, char *dir, char *file)
{
    if (path == NULL || dir == NULL || file == NULL)
        return 0;

    int pathlen = path_len(path);

    dir[0] = '\0';
    file[0] = '\0';

    char buffer[128];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (i < pathlen - 1)
        {
            strcat(dir, buffer);
            strapd(dir, '/');
        }
        else
        {
            strcpy(file, buffer);
        }
    }

    return 1;
}