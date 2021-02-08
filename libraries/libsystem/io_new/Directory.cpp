#include <libsystem/io_new/Directory.h>

namespace System
{

void Directory::read_entries()
{
    DirectoryEntry entry;

    auto read_result = _handle.read(&entry, sizeof(entry));
    while (read_result.success() && *read_result > 0)
    {
        _entries.push_back({entry.name, entry.stat});
        read_result = _handle.read(&entry, sizeof(entry));
    }

    _entries.sort([](auto &left, auto &right) {
        return strcmp(left.name.cstring(), right.name.cstring());
    });
}

Directory::Directory(const char *path)
    : _handle(path, OPEN_READ | OPEN_DIRECTORY),
      _path{Path::parse(path)}
{
    read_entries();
}

Directory::Directory(String path)
    : _handle(path, OPEN_READ | OPEN_DIRECTORY),
      _path{Path::parse(path)}
{
    read_entries();
}

Directory::Directory(const Path &path)
    : _handle(path.string(), OPEN_READ | OPEN_DIRECTORY),
      _path{path}
{
    read_entries();
}

Directory::Directory(System::Handle &&handle)
    : _handle{move(handle)}
{
    read_entries();
}

bool Directory::exist()
{
    return _handle.valid();
}

} // namespace System