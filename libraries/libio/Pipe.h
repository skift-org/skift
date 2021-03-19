#pragma once

#include <abi/Syscalls.h>
#include <libio/File.h>

namespace IO
{

struct Pipe
{
    RefPtr<Handle> reader;
    RefPtr<Handle> writer;

    static ResultOr<Pipe> create()
    {
        int reader_handle = HANDLE_INVALID_ID;
        int writer_handle = HANDLE_INVALID_ID;

        TRY(hj_create_pipe(&reader_handle, &writer_handle));

        return Pipe{
            make<Handle>(reader_handle),
            make<Handle>(writer_handle),
        };
    }
};

} // namespace IO