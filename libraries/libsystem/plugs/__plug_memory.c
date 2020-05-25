
#include <abi/Syscalls.h>

#include <libsystem/Logger.h>
#include <libsystem/Memory.h>
#include <libsystem/Result.h>

Result shared_memory_alloc(size_t size, uintptr_t *out_address)
{
    return (Result)__syscall(SYS_SHARED_MEMORY_ALLOC, size, (int)out_address, 0, 0, 0);
}

Result shared_memory_free(uintptr_t address)
{
    return (Result)__syscall(SYS_SHARED_MEMORY_FREE, address, 0, 0, 0, 0);
}

Result shared_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return (Result)__syscall(SYS_SHARED_MEMORY_INCLUDE, handle, (int)out_address, (int)out_size, 0, 0);
}

Result shared_memory_get_handle(uintptr_t address, int *out_handle)
{
    return (Result)__syscall(SYS_SHARED_MEMORY_GET_HANDLE, address, (int)out_handle, 0, 0, 0);
}
