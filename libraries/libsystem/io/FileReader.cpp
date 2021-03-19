#include <libsystem/io/FileReader.h>

FileReader::FileReader(const char *path) : _handle{make<IO::Handle>(path, OPEN_READ | OPEN_STREAM)}
{
}

FileReader::FileReader(Path &path) : _handle{make<IO::Handle>(path.string(), OPEN_READ | OPEN_STREAM)}
{
}

FileReader::FileReader(RefPtr<IO::Handle> handle) : _handle{handle}
{
}

size_t FileReader::seek(IO::SeekFrom from)
{
    return _handle->seek(from).value();
}

size_t FileReader::read(void *buffer, size_t size)
{
    return _handle->read(buffer, size).value_or_default(0);
}

size_t FileReader::length()
{
    auto result_or_stat = _handle->stat();

    if (result_or_stat.success())
    {
        return result_or_stat.value().size;
    }
    else
    {
        return 0;
    }
}

size_t FileReader::position()
{
    return _handle->tell().value_or_default(0);
}