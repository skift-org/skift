#pragma once
#include <libsystem/io/Stream.h>
#include <libsystem/io/Writer.h>
#include <libutils/Path.h>

class MemoryWriter final : public Writer
{
public:
    MemoryWriter(size_t reserve = 0);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual void flush() override;
    virtual size_t write(const void *buffer, size_t size) override;

    void clear();
    void pop_front(size_t size);

    const Vector<uint8_t> &data()
    {
        return _data;
    }

private:
    Vector<uint8_t> _data;
};