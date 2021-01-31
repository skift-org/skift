#pragma once

#include <abi/Filesystem.h>
#include <libsystem/io/Reader.h>

class Writer;
class SeekableReader : public Reader
{
public:
    virtual size_t seek(size_t pos, Whence whence) = 0;

    virtual void copy_to(Writer &writer) override;
};