#pragma once

#include <libio/Handle.h>
#include <libsystem/io/Writer.h>
#include <libutils/Path.h>

class FileWriter final : public Writer
{
private:
    RefPtr<IO::Handle> _handle;

public:
    FileWriter(const char *path);
    FileWriter(Path &path);
    FileWriter(RefPtr<IO::Handle> handle);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual void flush() override;
    virtual size_t write(const void *buffer, size_t size) override;
};