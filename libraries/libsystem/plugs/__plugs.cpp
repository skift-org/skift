
#include <assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/Memory.h>
#include <libc/skift/Lock.h>

static Lock _logger_lock{"logger_lock"};

void __plug_initialize()
{

}

void __plug_uninitialize(int exit_code)
{
    __unused(exit_code);
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