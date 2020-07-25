
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/thread/Lock.h>

Lock memlock;
Lock loglock;

Stream *in_stream;
Stream *out_stream;
Stream *err_stream;
Stream *log_stream;

extern "C" void _init();
extern "C" void _fini();

void __plug_init(void)
{
    lock_init(memlock);
    lock_init(loglock);

    // Open io stream
    in_stream = stream_open_handle(0, OPEN_READ);
    out_stream = stream_open_handle(1, OPEN_WRITE | OPEN_BUFFERED);
    err_stream = stream_open_handle(2, OPEN_WRITE | OPEN_BUFFERED);
    log_stream = stream_open_handle(3, OPEN_WRITE | OPEN_BUFFERED);

    _init();

    extern void (*__init_array_start[])(int, char **, char **) __attribute__((visibility("hidden")));
    extern void (*__init_array_end[])(int, char **, char **) __attribute__((visibility("hidden")));

    const size_t size = __init_array_end - __init_array_start;
    for (size_t i = 0; i < size; i++)
        (*__init_array_start[i])(0, NULL, NULL);
}

void __plug_fini(int exit_code)
{
    _fini();

    if (in_stream)
    {
        stream_flush(out_stream);
    }

    if (out_stream)
    {
        stream_flush(out_stream);
    }

    if (err_stream)
    {
        stream_flush(err_stream);
    }

    if (log_stream)
    {
        stream_flush(log_stream);
    }

    process_exit(exit_code);
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    logger_fatal("Assert failed: %s in %s:%s() ln%d!", expr, file, function, line);
    process_exit(-1);
}

void __plug_lock_assert_failed(Lock *lock, const char *file, const char *function, int line)
{
    logger_fatal("Lock assert failed: %s hold by %d in %s:%s() ln%d!", lock->name, lock->holder, file, function, line);
    process_exit(-1);
}

void __plug_logger_lock(void)
{
    lock_acquire(loglock);
}

void __plug_logger_unlock(void)
{
    lock_release(loglock);
}

void __no_return __plug_logger_fatal(void)
{
    stream_format(err_stream, "Fatal error occurred (see logs)!\n");
    __plug_process_exit(-1);
}

int __plug_memalloc_lock()
{
    lock_acquire(memlock);
    return 0;
}

int __plug_memalloc_unlock()
{
    lock_release(memlock);
    return 0;
}

void *__plug_memalloc_alloc(size_t size)
{
    uintptr_t address = 0;
    if (process_alloc(size, &address) == SUCCESS)
    {
        return (void *)address;
    }

    ASSERT_NOT_REACHED();
}

void __plug_memalloc_free(void *address, size_t size)
{
    process_free((uintptr_t)address, size);
}
