/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the userspace.   */

#include <string.h>
#include <skift/io.h>
#include <skift/lock.h>
#include <skift/process.h>
#include <skift/logger.h>
#include <skift/__plugs__.h>

lock_t memlock;
lock_t loglock;

iostream_t *in_stream;
iostream_t *out_stream;
iostream_t *err_stream;
iostream_t *log_stream;

void __plug_init(void)
{
    sk_lock_init(memlock);
    sk_lock_init(loglock);

    // Open io stream
    in_stream = NULL; // FIXME: no stdin,
    out_stream = iostream_open("/dev/console", IOSTREAM_WRITE | IOSTREAM_BUFFERED_WRITE);
    err_stream = iostream_open("/dev/console", IOSTREAM_WRITE | IOSTREAM_BUFFERED_WRITE);
    log_stream = iostream_open("/dev/serial",  IOSTREAM_WRITE | IOSTREAM_BUFFERED_WRITE);
}

void __plug_fini(int exit_code)
{
    iostream_flush(out_stream);
    iostream_flush(err_stream);
    iostream_flush(log_stream);

    __syscall(SYS_PROCESS_EXIT, exit_code, 0, 0, 0, 0);
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

void *__plug_memalloc_alloc(uint size)
{
    uint addr = sk_process_alloc(size);
    return (void *)addr;
}

int __plug_memalloc_free(void *memory, uint size)
{
    return sk_process_free((unsigned int)memory, size);
}