#pragma once

#include <libutils/Path.h>
#include <libutils/ResultOr.h>
#include <libutils/Slice.h>
#include <libutils/String.h>

#include <libsystem/Common.h>
#include <libsystem/Result.h>

class File
{
private:
    Path _path;

public:
    const Path &path()
    {
        return _path;
    }

    File(const char *path) : File(Path::parse(path)) {}

    File(String path) : File(Path::parse(path)) {}

    File(Path path) : _path(path) {}

    Result read_all(void **buffer, size_t *size);

    ResultOr<Slice> read_all();

    Result write_all(const void *buffer, size_t size);

    bool exist();

    Result copy(const char *destination);
};
