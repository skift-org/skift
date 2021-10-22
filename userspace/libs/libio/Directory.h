#pragma once

#include <libio/Handle.h>
#include <libio/Path.h>

namespace IO
{

struct Directory :
    public RawHandle
{
public:
    struct Entry
    {
        String name;
        HjStat stat;
    };

private:
    Ref<Handle> _handle;
    Opt<IO::Path> _path = NONE;
    Vec<Entry> _entries;

    HjResult read_entries();

public:
    const Opt<IO::Path> &path() { return _path; }
    const Vec<Entry> &entries() { return _entries; }

    Directory() {}
    Directory(const char *path);
    Directory(String path);
    Directory(const IO::Path &path);
    Directory(Ref<Handle> handle);

    Ref<Handle> handle() override { return _handle; }

    bool exist();
};

} // namespace IO
