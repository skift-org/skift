#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>

void memory_zero(void *where, size_t how_many);

void memory_set(void *where, char what, size_t how_many);

void memory_copy(void *from, size_t from_size, void *to, size_t to_size);

void memory_move(void *from, size_t from_size, void *to, size_t to_size);

Result memory_alloc(size_t size, uintptr_t *out_address);

Result memory_free(uintptr_t address);

Result memory_include(int handle, uintptr_t *out_address, size_t *out_size);

Result memory_get_handle(uintptr_t address, int *out_handle);
