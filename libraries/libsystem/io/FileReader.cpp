#include <libsystem/io/FileReader.h>

FileReader::FileReader(const char *path) : _handle{path, OPEN_READ | OPEN_STREAM}
{
}

FileReader::FileReader(Path &path) : _handle{path.string(), OPEN_READ | OPEN_STREAM}
{
}

FileReader::FileReader(System::Handle &&handle) : _handle{move(handle)}
{
}

size_t FileReader::seek(size_t pos, Whence whence)
{
    _handle.seek(pos, whence);
    return pos;
}

size_t FileReader::read(void *buffer, size_t size)
{
    auto result_or_read = _handle.read(buffer, size);

    if (result_or_read)
    {
        return *result_or_read;
    }
    else
    {
        return 0;
    }
}

size_t FileReader::length()
{
    auto result_or_stat = _handle.stat();

    if (result_or_stat)
    {
        return result_or_stat->size;
    }
    else
    {
        return 0;
    }
}

size_t FileReader::position()
{
    auto result_or_tell = _handle.tell();

    if (result_or_tell)
    {
        return *result_or_tell;
    }
    else
    {
        return 0;
    }
}