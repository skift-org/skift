#include <abi/Syscalls.h>

#include <libsystem/Result.h>
#include <libsystem/system/Memory.h>

Result memory_alloc(size_t size, uintptr_t *out_address)
{
    return hj_memory_alloc(size, out_address);
}

Result memory_free(uintptr_t address)
{
    return hj_memory_free(address);
}

Result memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return hj_memory_include(handle, out_address, out_size);
}

Result memory_get_handle(uintptr_t address, int *out_handle)
{
    return hj_memory_get_handle(address, out_handle);
}
