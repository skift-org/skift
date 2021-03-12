#pragma once

#include <abi/Filesystem.h>
#include <libio/Seek.h>
#include <libsystem/io/Reader.h>

class Writer;
class SeekableReader : public Reader
{
public:
    virtual size_t seek(IO::SeekFrom from) = 0;

    virtual void copy_to(Writer &writer) override;
};