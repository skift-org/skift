#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

void memory_zero(void *where, uint how_many);

void memory_set(void *where, byte what, uint how_many);

void memory_uset(void *where, ubyte what, uint how_many);

void memory_copy(void *from, uint from_size, void *to, uint to_size);

void memory_move(void *from, uint from_size, void *to, uint to_size);

int shared_memory_alloc(uint how_many_pages);

int shared_memory_acquire(int shm, uint *addr);

int shared_memory_release(int shm);
