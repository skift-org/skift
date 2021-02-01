#pragma once
#include <libsystem/io/Reader.h>
#include <libsystem/math/MinMax.h>

class ScopedReader final : public Reader
{
public:
    ScopedReader(Reader &reader, size_t size);

    virtual size_t length() override;
    virtual size_t position() override;
    virtual size_t read(void *buffer, size_t size) override;

private:
    size_t _start_pos = 0;
    size_t _size = 0;
    Reader &_reader;
};