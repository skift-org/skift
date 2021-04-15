#pragma once

#include <libio/Handle.h>
#include <libio/Path.h>

namespace IO
{

class Directory :
    public RawHandle
{
public:
    struct Entry
    {
        String name;
        FileState stat;
    };

private:
    RefPtr<Handle> _handle;
    Optional<IO::Path> _path;
    Vector<Entry> _entries;

    Result read_entries();

public:
    const Optional<IO::Path> &path() { return _path; }
    const Vector<Entry> &entries() { return _entries; }

    Directory() {}
    Directory(const char *path);
    Directory(String path);
    Directory(const IO::Path &path);
    Directory(RefPtr<Handle> handle);

    RefPtr<Handle> handle() override { return _handle; }

    bool exist();
};

} // namespace IO
