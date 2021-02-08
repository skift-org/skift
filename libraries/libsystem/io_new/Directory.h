#pragma once

#include <libsystem/Handle.h>
#include <libutils/Path.h>

namespace System
{

class Directory
{
public:
    struct Entry
    {
        String name;
        FileState stat;
    };

private:
    System::Handle _handle;
    Optional<Path> _path;
    Vector<Entry> _entries;

    void read_entries();

public:
    const Optional<Path> &path() { return _path; }
    const Vector<Entry> &entries() { return _entries; }

    Directory(const char *path);
    Directory(String path);
    Directory(const Path &path);
    Directory(System::Handle &&handle);

    bool exist();
};

} // namespace System
