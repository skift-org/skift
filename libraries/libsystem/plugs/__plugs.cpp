
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/Memory.h>
#include <libsystem/thread/Lock.h>

#include <libsystem/cxx/cxx.h>

static Lock _memory_lock{"memory_lock"};
static Lock _logger_lock{"logger_lock"};

Stream *in_stream;
Stream *out_stream;
Stream *err_stream;
Stream *log_stream;

extern "C" void _init();
extern "C" void _fini();

void __plug_initialize()
{
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
    {

        (*__init_array_start[i])(0, nullptr, nullptr);
    }
}

void __plug_uninitialize(int exit_code)
{
    _fini();
    __cxa_finalize(nullptr);

    if (in_stream)
    {
        stream_flush(in_stream);
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

    __plug_process_exit(exit_code);
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    logger_fatal("Assert failed: %s in %s:%s() ln%d!", expr, file, function, line);
    process_abort();
}

void __plug_logger_lock()
{
    _logger_lock.acquire(SOURCE_LOCATION);
}

void __plug_logger_unlock()
{
    _logger_lock.release(SOURCE_LOCATION);
}

void __no_return __plug_logger_fatal()
{
    stream_format(err_stream, "Fatal error occurred (see logs)!\n");
    process_abort();
}

void __plug_memory_lock()
{
    _memory_lock.acquire(SOURCE_LOCATION);
}

void __plug_memory_unlock()
{
    _memory_lock.release(SOURCE_LOCATION);
}

void *__plug_memory_alloc(size_t size)
{
    uintptr_t address = 0;
    assert(memory_alloc(size, &address) == SUCCESS);
    return (void *)address;
}

void __plug_memory_free(void *address, size_t size)
{
    __unused(size);
    memory_free((uintptr_t)address);
}
