
#include <abi/Syscalls.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/system/Memory.h>

Result memory_alloc(size_t size, uintptr_t *out_address)
{
    return (Result)__syscall(SYS_MEMORY_ALLOC, size, (int)out_address, 0, 0, 0);
}

Result memory_free(uintptr_t address)
{
    return (Result)__syscall(SYS_MEMORY_FREE, address, 0, 0, 0, 0);
}

Result memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return (Result)__syscall(SYS_MEMORY_INCLUDE, handle, (int)out_address, (int)out_size, 0, 0);
}

Result memory_get_handle(uintptr_t address, int *out_handle)
{
    return (Result)__syscall(SYS_MEMORY_GET_HANDLE, address, (int)out_handle, 0, 0, 0);
}
