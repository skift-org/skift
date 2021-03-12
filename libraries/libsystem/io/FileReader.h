#pragma once

#include <libio/Handle.h>
#include <libsystem/io/SeekableReader.h>

#include <libutils/Path.h>

class FileReader final : public SeekableReader
{
private:
    RefPtr<IO::Handle> _handle;

public:
    FileReader(const char *path);
    FileReader(Path &path);
    FileReader(RefPtr<IO::Handle> handle);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual size_t read(void *buffer, size_t size) override;
    virtual size_t seek(IO::SeekFrom from) override;
};