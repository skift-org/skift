#include <assert.h>
#include <stdio.h>
#include <string.h>

// TODO: read this from system settings & env variable whatever
#define TEMP_DIR "/Temp/"

char *tmpnam(const char *pfx)
{
    return tempnam(NULL, pfx);
}

char *tempnam(const char *dir, const char *pfx)
{
    int plen;
    if (pfx == NULL || !pfx[0])
    {
        pfx = "file";
        plen = 4;
    }
    else
    {
        plen = strlen(pfx);
        if (plen > 5)
        {
            plen = 5;
        }
    }

    if (dir == NULL)
    {
        dir = TEMP_DIR;
    }
    else
    {
        assert(false);
    }

    // Unique number
    static unsigned int num = 0;
    char num_buf[6];
    snprintf(num_buf, 6, "%u", num);
    num++;

    char out_path[256];
    // First append the directory
    strcat(out_path, TEMP_DIR);
    // Then the prefix
    strcat(out_path, pfx);
    // then the unique number
    strcat(out_path, num_buf);

    return strdup(out_path);
}