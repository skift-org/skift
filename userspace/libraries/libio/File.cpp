#include <libio/File.h>

namespace IO
{

File::File(const char *path, OpenFlag flags)
    : _handle{make<Handle>(path, flags | OPEN_STREAM)},
      _path{IO::Path::parse(path)}
{
}

File::File(String path, OpenFlag flags)
    : _handle{make<Handle>(path, flags | OPEN_STREAM)},
      _path{IO::Path::parse(path)}
{
}

File::File(IO::Path &path, OpenFlag flags)
    : _handle{make<Handle>(path.string(), flags | OPEN_STREAM)},
      _path{path}
{
}

File::File(RefPtr<Handle> handle)
    : _handle{handle}
{
}

ResultOr<size_t> File::read(void *buffer, size_t size)
{
    return _handle->read(buffer, size);
}

ResultOr<size_t> File::write(const void *buffer, size_t size)
{
    return _handle->write(buffer, size);
}

ResultOr<size_t> File::call(IOCall call, void *args)
{
    return _handle->call(call, args);
}

ResultOr<size_t> File::seek(SeekFrom from)
{
    auto seek_result = _handle->seek(from);

    if (seek_result.success())
    {
        return (size_t)seek_result.unwrap();
    }
    else
    {
        return seek_result.result();
    }
}

ResultOr<size_t> File::tell()
{
    // FIXME: sketchy cast
    return (size_t)_handle->tell().unwrap();
}

ResultOr<size_t> File::length()
{
    auto result_or_stat = _handle->stat();

    if (result_or_stat.success())
    {
        return result_or_stat.unwrap().size;
    }
    else
    {
        return 0;
    }
}

bool File::exist()
{
    return _handle->valid();
}

} // namespace IO