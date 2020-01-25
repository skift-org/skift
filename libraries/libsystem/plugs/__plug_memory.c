/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/memory.h>
#include <abi/Syscalls.h>
#include <libsystem/error.h>

int shared_memory_alloc(uint pagecount)
{
    int r = __syscall(SYS_SHARED_MEMORY_ALLOC, pagecount, 0, 0, 0, 0);
    RETURN_AND_SET_ERROR(r, r, -1);
}

int shared_memory_acquire(int shm, uint *addr)
{
    int r = __syscall(SYS_SHARED_MEMORY_ALLOC, shm, (int)addr, 0, 0, 0);
    RETURN_AND_SET_ERROR(r, r, -1);
}

int shared_memory_release(int shm)
{
    int r = __syscall(SYS_SHARED_MEMORY_ALLOC, shm, 0, 0, 0, 0);
    RETURN_AND_SET_ERROR(r, r, -1);
}