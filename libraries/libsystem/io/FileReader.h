#pragma once

#include <libsystem/Handle.h>
#include <libsystem/io/SeekableReader.h>

#include <libutils/Path.h>

class FileReader final : public SeekableReader
{
private:
    system::Handle _handle;

public:
    FileReader(const char *path);
    FileReader(Path &path);
    FileReader(system::Handle &&handle);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual size_t read(void *buffer, size_t size) override;
    virtual size_t seek(size_t pos, Whence whence) override;
};