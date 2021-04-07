
#include <assert.h>
#include <skift/Lock.h>

#include <libsystem/Logger.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/Memory.h>

static Lock _logger_lock{"logger_lock"};

void __plug_initialize()
{
}

void __plug_uninitialize(int exit_code)
{
    UNUSED(exit_code);
}

void __plug_logger_lock()
{
    _logger_lock.acquire();
}

void __plug_logger_unlock()
{
    _logger_lock.release();
}

void NO_RETURN __plug_logger_fatal(const char *message)
{
    stream_format(err_stream, "Fatal error occurred: %s!\n", message);
    process_abort();
}