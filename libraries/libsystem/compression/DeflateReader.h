#pragma once
#include <libsystem/compression/Inflate.h>
#include <libsystem/io/MemoryWriter.h>
#include <libsystem/io/Reader.h>
#include <libutils/Vector.h>

class DeflateReader : public Reader
{
    DeflateReader(Reader &reader);

    virtual size_t length() override;
    virtual size_t position() override;

    virtual size_t read(void *buffer, size_t size) override;

private:
    MemoryWriter _mem_buffer;
    Reader &_reader;
    Inflate _inflate;
    size_t _position = 0;
};