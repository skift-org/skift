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
    auto result_or_read = _handle->read(buffer, size);

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
    auto result_or_stat = _handle->stat();

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
    auto result_or_tell = _handle->tell();

    if (result_or_tell)
    {
        return *result_or_tell;
    }
    else
    {
        return 0;
    }
}