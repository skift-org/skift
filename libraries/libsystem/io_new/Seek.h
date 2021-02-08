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
        auto original_position = seek(0, WHENCE_HERE);

        if (!original_position)
        {
            return original_position.result();
        }

        auto end_position = seek(0, WHENCE_END);

        if (!end_position)
        {
            return end_position.result();
        }

        auto back_to_original_position = seek(*original_position, WHENCE_START);

        if (!back_to_original_position)
        {
            return back_to_original_position.result();
        }

        return *end_position;
    }
};

} // namespace System