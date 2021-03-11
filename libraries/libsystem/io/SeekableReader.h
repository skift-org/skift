#pragma once

#include <abi/Filesystem.h>
#include <libsystem/io/Reader.h>
#include <libsystem/io_new/Seek.h>

class Writer;
class SeekableReader : public Reader
{
public:
    virtual size_t seek(System::SeekFrom from) = 0;

    virtual void copy_to(Writer &writer) override;
};