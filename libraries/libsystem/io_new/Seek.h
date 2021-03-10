#pragma once

#include <abi/Handle.h>
#include <libutils/ResultOr.h>

namespace System
{

class Seek
{
public:
    virtual ResultOr<size_t> seek(size_t pos, Whence whence) = 0;
    virtual ResultOr<size_t> tell() = 0;

    virtual ResultOr<size_t> length()
    {
        auto original_position = TRY(seek(0, WHENCE_HERE));

        auto end_position = TRY(seek(0, WHENCE_END));

        TRY(seek(original_position, WHENCE_START));

        return end_position;
    }
};

} // namespace System