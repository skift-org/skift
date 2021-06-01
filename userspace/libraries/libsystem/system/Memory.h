#pragma once

#include <abi/Result.h>
#include <libutils/Prelude.h>

HjResult memory_alloc(size_t size, uintptr_t *out_address);

HjResult memory_free(uintptr_t address);

HjResult memory_include(int handle, uintptr_t *out_address, size_t *out_size);

HjResult memory_get_handle(uintptr_t address, int *out_handle);
