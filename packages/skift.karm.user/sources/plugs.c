/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the userspace.   */

#include <string.h>
#include <skift/io.h>
#include <skift/lock.h>
#include <skift/process.h>
#include <skift/logger.h>
#include <skift/formatter.h>
#include <skift/__plugs.h>

lock_t memlock;
lock_t loglock;

void __plug_init(void)
{
    sk_lock_init(memlock);
    sk_lock_init(loglock);
    sk_formatter_init();
}

int __plug_print(const char *buffer)
{
    sk_io_print(buffer);
    return strlen(buffer);
}

void __plug_putchar(int c)
{
    UNUSED(c);
    sk_log(LOG_ERROR, "__plug_putchar() not implemented!");
}

int __plug_getchar()
{
    sk_log(LOG_ERROR, "__plug_getchar() not implemented!");
    return EOF;
}

void __plug_read(char * buffer, uint size)
{
    UNUSED(buffer);
    UNUSED(size);

    sk_log(LOG_ERROR, "__plug_read() not implemented!");
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    sk_log(LOG_FATAL, "assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    sk_process_exit(-1);
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
    return (void*)addr;
}

int __plug_memalloc_free(void* memory, uint size)
{
    return sk_process_free((unsigned int)memory, size);
}