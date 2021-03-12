#pragma once
#include <libsystem/io/SeekableReader.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

class MemoryReader final : public SeekableReader
{
public:
    MemoryReader(const uint8_t *data, size_t size);
    MemoryReader(const Vector<uint8_t> &buffer);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual size_t read(void *buffer, size_t size) override;
    virtual size_t seek(IO::SeekFrom from) override;

private:
    const uint8_t *_data;
    size_t _size;
    size_t _position = 0;
};