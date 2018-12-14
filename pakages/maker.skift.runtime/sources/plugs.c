/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the userspace.   */

#include <string.h>
#include <skift/io.h>
#include <skift/lock.h>
#include <skift/process.h>
#include <skift/__plugs.h>

lock_t memlock;
lock_t loglock;


void __plug_init(void)
{
    sk_lock_init(memlock);
    sk_lock_init(loglock);
}

int __plug_print(const char *buffer)
{
    sk_io_print(buffer);
    return strlen(buffer);
}

void __plug_putchar(int c)
{
    printf("__plug_putchar() not implemented!");
}

int __plug_getchar()
{
    printf("__plug_getchar() not implemented!");
    return EOF;
}

void __plug_read(char * buffer, uint size)
{
    UNUSED(buffer);
    UNUSED(size);

    printf("__plug_read() not implemented!");
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    printf("assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
}

int __plug_logger_lock()
{
    sk_lock_acquire(loglock);
    return 0;
}

int __plug_logger_unlock()
{
    sk_lock_release(loglock);
    return 0;
}

int __plug_memalloc_lock()
{
    sk_lock_acquire(memlock);
    return 0;
}

int __plug_memalloc_unlock()
{
    sk_lock_release(memlock);
    return 0;
}

void* __plug_memalloc_alloc(uint size)
{
    uint addr = sk_process_alloc(size);
    printf("LIBALLOC ALOCC %x", addr);
    return (void*)addr;
}

int __plug_memalloc_free(void* memory, uint size)
{
    return sk_process_free((unsigned int)memory, size);
}