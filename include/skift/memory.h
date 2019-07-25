#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

int shared_memory_alloc(int pagecount);

int shared_memory_acquire(int shm, uint *addr);

int shared_memory_release(int shm);
