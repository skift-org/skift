#include <libfile/TARArchive.h>
#include <libio/File.h>

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

    char __padding[12];

    size_t file_size()
    {
        size_t size = 0;
        size_t count = 1;

        for (size_t j = 11; j > 0; j--, count *= 8)
        {
            size += ((this->size[j - 1] - '0') * count);
        }

        return size;
    }
};

bool tar_read(void *tarfile, TARBlock *block, size_t index)
{
    TARRawBlock *header = (TARRawBlock *)tarfile;

    for (size_t i = 0; i < index; i++)
    {
        if (header->name[0] == '\0')
        {
            return false;
        }

        size_t size = header->file_size();

        header = (TARRawBlock *)((char *)header + ((size / 512) + 1) * 512);

        if (size % 512)
        {
            header = (TARRawBlock *)((char *)header + 512);
        }
    }

    if (header->name[0] == '\0')
    {
        return false;
    }

    memcpy(block->name, header->name, 100);
    block->size = header->file_size();
    block->typeflag = header->typeflag;
    memcpy(block->linkname, header->linkname, 100);
    block->data = (char *)header + 512;

    return true;
}

#ifndef __KERNEL__

#    include <libfile/TARArchive.h>
#    include <libsystem/io/FileReader.h>

TARArchive::TARArchive(Path path, bool read) : Archive(path)
{
    if (read)
    {
        read_archive();
    }
}

Result TARArchive::extract(unsigned int entry_index, const char *dest_path)
{
    __unused(entry_index);
    __unused(dest_path);

    return ERR_FUNCTION_NOT_IMPLEMENTED;
}

Result TARArchive::insert(const char *entry_name, const char *src_path)
{
    __unused(entry_name);
    __unused(src_path);

    return ERR_FUNCTION_NOT_IMPLEMENTED;
}

Result TARArchive::read_archive()
{
    _valid = false;

    IO::File archive_file{_path};

    // Archive does not exist
    if (!archive_file.exist())
    {
        logger_error("Archive does not exist: %s", _path.string().cstring());
        return SUCCESS;
    }

    logger_trace("Opening file: '%s'", _path.string().cstring());

    TARRawBlock block;
    while (TRY(archive_file.read(&block, sizeof(TARRawBlock))) == sizeof(TARRawBlock))
    {
        if (block.name[0] == '\0')
        {
            _valid = true;
            return SUCCESS;
        }

        _entries.push_back({
            String{block.name},
            block.file_size(),
            block.file_size(),
            TRY(archive_file.tell()),
            0,
        });

        TRY(archive_file.seek(IO::SeekFrom::current(__align_up(block.file_size(), 512))));
    }

    return SUCCESS;
}

#endif