#pragma once

#include <abi/Handle.h>
#include <libutils/ResultOr.h>

namespace IO
{

enum class Whence : uint8_t
{
    START = HJ_WHENCE_START,
    CURRENT = HJ_WHENCE_CURRENT,
    END = HJ_WHENCE_END,
};

struct SeekFrom
{
    Whence whence;
    ssize64_t position;

    static SeekFrom start(ssize64_t position = 0)
    {
        return {Whence::START, position};
    }

    static SeekFrom current(ssize64_t position = 0)
    {
        return {Whence::CURRENT, position};
    }

    static SeekFrom end(ssize64_t position = 0)
    {
        return {Whence::END, position};
    }
};

struct Seek
{
    virtual ~Seek() {}

    virtual ResultOr<size_t> seek(SeekFrom from) = 0;
    virtual ResultOr<size_t> tell() = 0;

    virtual ResultOr<size_t> length()
    {
        auto original_position = TRY(seek(SeekFrom::current(0)));

        auto end_position = TRY(seek(SeekFrom::end(0)));

        TRY(seek(SeekFrom::start(original_position)));

        return end_position;
    }
};

} // namespace IO