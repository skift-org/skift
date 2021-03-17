#pragma once

#include <abi/Syscalls.h>
#include <libio/File.h>

namespace IO
{

struct Terminal
{
    File client = nullptr;
    File server = nullptr;

    static ResultOr<Terminal> create()
    {
        int server_handle = HANDLE_INVALID_ID;
        int client_handle = HANDLE_INVALID_ID;

        TRY(hj_create_term(&server_handle, &client_handle));

        return Terminal{
            make<Handle>(client_handle),
            make<Handle>(server_handle),
        };
    }
};

} // namespace IO