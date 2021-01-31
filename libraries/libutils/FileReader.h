#pragma once
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>
#include <libutils/SeekableReader.h>

class FileReader final : public SeekableReader
{
public:
    FileReader(const char *path);
    FileReader(Path &path);

    ~FileReader();

    virtual size_t length() override;
    virtual size_t position() override;

    virtual size_t read(void *buffer, size_t size) override;
    virtual size_t seek(size_t pos, Whence whence) override;

private:
    Stream *_file = nullptr;
};