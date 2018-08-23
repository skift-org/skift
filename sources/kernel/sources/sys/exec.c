#include "kernel/filesystem.h"
#include "kernel/logging.h"
#include "libelf.h"

int exec(char * path)
{
    file_t* file = file_open(NULL, path);

    if (file == NULL)
    {
        debug("%s file not found!", path);
        return 1;
    }

    return 0;
}