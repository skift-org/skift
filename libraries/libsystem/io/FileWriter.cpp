#include <libsystem/io/FileWriter.h>

FileWriter::FileWriter(const char *path) : _file(stream_open(path, OPEN_WRITE | OPEN_CREATE))
{
    assert(_file);
}

FileWriter::FileWriter(Path &path) : _file(stream_open(path.string().cstring(), OPEN_WRITE | OPEN_CREATE))
{
    assert(_file);
}

FileWriter::~FileWriter()
{
    stream_close(_file);
}

void FileWriter::flush()
{
    stream_flush(_file);
}

void FileWriter::write(const void *buffer, size_t size)
{
    stream_write(_file, buffer, size);
}

size_t FileWriter::length()
{
    FileState stat;
    stream_stat(_file, &stat);
    return stat.size;
}

size_t FileWriter::position()
{
    return stream_tell(_file, WHENCE_START);
}