#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/runtime.h>

void memory_zero(void *where, uint how_many);

void memory_set(void *where, byte what, uint how_many);

void memory_uset(void *where, ubyte what, uint how_many);

void memory_copy(void *from, uint from_size, void *to, uint to_size);

void memory_move(void *from, uint from_size, void *to, uint to_size);

Result shared_memory_alloc(size_t size, uintptr_t *out_address);

Result shared_memory_free(uintptr_t address);

Result shared_memory_include(int handle, uintptr_t *out_address, size_t *out_size);

Result shared_memory_get_handle(uintptr_t address, int *out_handle);
