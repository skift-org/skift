
#include <abi/Syscalls.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/system/Memory.h>

Result memory_alloc(size_t size, uintptr_t *out_address)
{
    return __syscall(SYS_MEMORY_ALLOC, size, (uintptr_t)out_address);
}

Result memory_free(uintptr_t address)
{
    return __syscall(SYS_MEMORY_FREE, address);
}

Result memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return __syscall(SYS_MEMORY_INCLUDE, handle, (uintptr_t)out_address, (uintptr_t)out_size);
}

Result memory_get_handle(uintptr_t address, int *out_handle)
{
    return __syscall(SYS_MEMORY_GET_HANDLE, address, (uintptr_t)out_handle);
}
