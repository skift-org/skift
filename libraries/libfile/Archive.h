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
    };

    Archive(File file) : _file(file)
    {
    }

    virtual unsigned int get_entry_count() = 0;
    virtual Entry get_entry(unsigned int entry_index) = 0;

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