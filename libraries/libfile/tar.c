/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* tar.c: read in memory tar archive                                          */

#include <libsystem/cstring.h>
#include <libfile/tar.h>

typedef struct __packed
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
} tar_rawblock_t;

uint get_file_size(tar_rawblock_t *header)
{
    unsigned int size = 0;
    unsigned int count = 1;

    for (uint j = 11; j > 0; j--, count *= 8)
    {
        size += ((header->size[j - 1] - '0') * count);
    }

    return size;
}

uint tar_count(void *tarfile)
{
    tar_rawblock_t *header = (tar_rawblock_t *)tarfile;
    uint count = 0;

    while (header->name[0] != '\0')
    {
        count++;

        u32 size = get_file_size(header);

        header = (tar_rawblock_t *)((char *)header + ((size / 512) + 1) * 512);

        if (size % 512)
            header = (tar_rawblock_t *)((char *)header + 512);
    }

    return count;
}

bool tar_read(void *tarfile, tar_block_t *block, uint index)
{
    tar_rawblock_t *header = (tar_rawblock_t *)tarfile;

    for (size_t i = 0; i < index; i++)
    {
        if (header->name[0] == '\0')
            return false;

        u32 size = get_file_size(header);

        header = (tar_rawblock_t *)((char *)header + ((size / 512) + 1) * 512);

        if (size % 512)
            header = (tar_rawblock_t *)((char *)header + 512);
    }

    if (header->name[0] == '\0')
        return false;

    memcpy(block->name, header->name, 100);
    block->size = get_file_size(header);
    block->data = (char *)header + 512;

    return true;
}
