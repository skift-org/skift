#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/io/File.h>

class Archive : public RefCounted<Archive>
{
public:
    struct Entry
    {
        String name;
        size_t uncompressed_size;
        size_t compressed_size;
        size_t archive_offset;
        unsigned int compression;
    };

    Archive(File file) : _file(file)
    {
    }

    inline const Vector<Entry> &entries()
    {
        return _entries;
    }

    virtual Result extract(unsigned int entry_index, const char *dest_path) = 0;
    virtual Result insert(const char *entry_name, const char *src_dir) = 0;

    inline File get_file()
    {
        return _file;
    }

    inline bool valid()
    {
        return _valid;
    }

protected:
    Vector<Entry> _entries;
    File _file;
    bool _valid = true;
};