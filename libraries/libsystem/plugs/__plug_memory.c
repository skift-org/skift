/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Syscalls.h>

#include <libsystem/Result.h>
#include <libsystem/logger.h>
#include <libsystem/memory.h>

Result shared_memory_alloc(size_t size, uintptr_t *out_address)
{
    return __syscall(SYS_SHARED_MEMORY_ALLOC, size, (int)out_address, 0, 0, 0);
}

Result shared_memory_free(uintptr_t address)
{
    return __syscall(SYS_SHARED_MEMORY_FREE, address, 0, 0, 0, 0);
}

Result shared_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return __syscall(SYS_SHARED_MEMORY_INCLUDE, handle, (int)out_address, (int)out_size, 0, 0);
}

Result shared_memory_get_handle(uintptr_t address, int *out_handle)
{
    return __syscall(SYS_SHARED_MEMORY_GET_HANDLE, address, (int)out_handle, 0, 0, 0);
}
