#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/io/File.h>

class Archive
{
public:
    struct Entry
    {
        String name;
        unsigned int uncompressed_size;
        unsigned int compressed_size;
        unsigned int archive_offset;
        unsigned int compression;
    };

    Archive(File file) : _file(file)
    {
    }

    inline unsigned int get_entry_count()
    {
        return _entries.count();
    }

    inline Entry &get_entry(unsigned int entry_index)
    {
        return _entries[entry_index];
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
    bool _valid = false;
};