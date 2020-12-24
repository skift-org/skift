
/* tar.c: read in memory tar archive                                          */

#include <libfile/tar.h>
#include <libsystem/core/CString.h>

struct __packed TARRawBlock
{
    char name[100];     /*   0 */
    char mode[8];       /* 100 */
    char uid[8];        /* 108 */
    char gid[8];        /* 116 */
    char size[12];      /* 124 */
    char mtime[12];     /* 136 */
    char chksum[8];     /* 148 */
    char typeflag;      /* 156 */
    char linkname[100]; /* 157 */
    char magic[6];      /* 257 */
    char version[2];    /* 263 */
    char uname[32];     /* 265 */
    char gname[32];     /* 297 */
    char devmajor[8];   /* 329 */
    char devminor[8];   /* 337 */
    char prefix[155];   /* 345 */
                        /* 500 */
};

size_t get_file_size(TARRawBlock *header)
{
    unsigned int size = 0;
    unsigned int count = 1;

    for (size_t j = 11; j > 0; j--, count *= 8)
    {
        size += ((header->size[j - 1] - '0') * count);
    }

    return size;
}

size_t tar_count(void *tarfile)
{
    TARRawBlock *header = (TARRawBlock *)tarfile;
    size_t count = 0;

    while (header->name[0] != '\0')
    {
        count++;

        size_t size = get_file_size(header);

        header = (TARRawBlock *)((char *)header + ((size / 512) + 1) * 512);

        if (size % 512)
            header = (TARRawBlock *)((char *)header + 512);
    }

    return count;
}

bool tar_read(void *tarfile, TARBlock *block, size_t index)
{
    TARRawBlock *header = (TARRawBlock *)tarfile;

    for (size_t i = 0; i < index; i++)
    {
        if (header->name[0] == '\0')
            return false;

        size_t size = get_file_size(header);

        header = (TARRawBlock *)((char *)header + ((size / 512) + 1) * 512);

        if (size % 512)
            header = (TARRawBlock *)((char *)header + 512);
    }

    if (header->name[0] == '\0')
        return false;

    memcpy(block->name, header->name, 100);
    block->size = get_file_size(header);
    block->typeflag = header->typeflag;
    memcpy(block->linkname, header->linkname, 100);
    block->data = (char *)header + 512;

    return true;
}
