#include <libsystem/io/FileReader.h>

FileReader::FileReader(const char *path) : _file(stream_open(path, OPEN_READ))
{
    assert(_file);
}

FileReader::FileReader(Path &path) : _file(stream_open(path.string().cstring(), OPEN_READ))
{
    assert(_file);
}

FileReader::~FileReader()
{
    stream_close(_file);
}

size_t FileReader::seek(size_t pos, Whence whence)
{
    stream_seek(_file, pos, whence);
    return pos;
}

size_t FileReader::read(void *buffer, size_t size)
{
    return stream_read(_file, buffer, size);
}

size_t FileReader::length()
{
    FileState stat;
    stream_stat(_file, &stat);
    return stat.size;
}

size_t FileReader::position()
{
    return stream_tell(_file, WHENCE_START);
}