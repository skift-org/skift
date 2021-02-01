#pragma once
#include <libsystem/io/Stream.h>
#include <libsystem/io/Writer.h>
#include <libutils/Path.h>

class FileWriter final : public Writer
{
public:
    FileWriter(const char *path);
    FileWriter(Path &path);

    ~FileWriter();

    virtual size_t length() override;
    virtual size_t position() override;

    virtual void flush() override;
    virtual void write(const void *buffer, size_t size) override;

private:
    Stream *_file;
};