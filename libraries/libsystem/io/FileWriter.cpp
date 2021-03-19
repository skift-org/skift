#include <libsystem/io/FileWriter.h>

FileWriter::FileWriter(const char *path)
    : _handle{make<IO::Handle>(path, OPEN_WRITE | OPEN_CREATE | OPEN_STREAM)}
{
}

FileWriter::FileWriter(Path &path)
    : _handle{make<IO::Handle>(path.string(), OPEN_WRITE | OPEN_CREATE | OPEN_STREAM)}
{
}

FileWriter::FileWriter(RefPtr<IO::Handle> handle)
    : _handle{handle}
{
}

void FileWriter::flush() {}

size_t FileWriter::write(const void *buffer, size_t size)
{
    return _handle->write(buffer, size).value_or_default(0);
}

size_t FileWriter::length()
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

size_t FileWriter::position()
{
    return _handle->tell().value_or_default(0);
}