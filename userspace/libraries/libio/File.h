#pragma once

#include <libio/Path.h>

#include <libio/Handle.h>
#include <libio/Reader.h>
#include <libio/Writer.h>

namespace IO
{

class File final :
    public Reader,
    public Writer,
    public Seek,
    public RawHandle
{
private:
    RefPtr<Handle> _handle;
    Optional<IO::Path> _path;

public:
    const Optional<IO::Path> &path() { return _path; }

    File() {}

    File(const char *path, OpenFlag flags = 0);

    File(String path, OpenFlag flags = 0);

    File(IO::Path &path, OpenFlag flags = 0);

    File(RefPtr<Handle> handle);

    ResultOr<size_t> read(void *buffer, size_t size) override;

    ResultOr<size_t> write(const void *buffer, size_t size) override;

    ResultOr<size_t> call(IOCall call, void *args);

    ResultOr<size_t> seek(SeekFrom from) override;

    ResultOr<size_t> tell() override;

    ResultOr<size_t> length() override;

    virtual RefPtr<Handle> handle() override { return _handle; }

    bool exist();

    Result result()
    {
        if (!_handle)
        {
            return ERR_BAD_HANDLE;
        }

        return _handle->result();
    }
};

} // namespace IO
