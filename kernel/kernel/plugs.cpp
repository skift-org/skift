#include <assert.h>
#include <skift/Plugs.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io/Stream_internal.h>
#include <libsystem/system/System.h>

#include "archs/Arch.h"

#include "kernel/graphics/EarlyConsole.h"
#include "kernel/interrupts/Interupts.h"
#include "kernel/memory/Memory.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Handles.h"
#include "kernel/tasking/Task-Launchpad.h"
#include "kernel/tasking/Task-Memory.h"

/* --- Framework initialization --------------------------------------------- */

Stream *in_stream = nullptr;
Stream *out_stream = nullptr;
Stream *err_stream = nullptr;
Stream *log_stream = nullptr;

#define INTERNAL_LOG_STREAM_HANDLE HANDLE_INVALID_ID
static Stream internal_log_stream = {};

void __plug_initialize()
{
    internal_log_stream.handle.id = INTERNAL_LOG_STREAM_HANDLE;

    in_stream = nullptr;
    out_stream = &internal_log_stream;
    err_stream = &internal_log_stream;
    log_stream = &internal_log_stream;
}

void __plug_uninitialize(int)
{
    ASSERT_NOT_REACHED();
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    logger_fatal("Assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    ASSERT_NOT_REACHED();
}

/* --- Systeme API ---------------------------------------------------------- */

TimeStamp __plug_system_get_time()
{
    return arch_get_time();
}

Tick __plug_system_get_ticks()
{
    return system_get_ticks();
}

/* --- Memory allocator plugs ----------------------------------------------- */

void __plug_memory_lock()
{
    interrupts_retain();
}

void __plug_memory_unlock()
{
    interrupts_release();
}

void *__plug_memory_alloc(size_t size)
{
    uintptr_t address = 0;
    assert(memory_alloc(arch_kernel_address_space(), size, MEMORY_CLEAR, &address) == SUCCESS);
    return (void *)address;
}

void __plug_memory_free(void *address, size_t size)
{
    memory_free(arch_kernel_address_space(), (MemoryRange){(uintptr_t)address, size});
}

/* --- Logger plugs --------------------------------------------------------- */

void __plug_logger_lock()
{
    interrupts_retain();
}

void __plug_logger_unlock()
{
    interrupts_release();
}

void NO_RETURN __plug_logger_fatal(const char *message)
{
    system_panic(message);
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this()
{
    return scheduler_running_id();
}

const char *__plug_process_name()
{
    if (scheduler_running())
    {
        return scheduler_running()->name;
    }
    else
    {
        return "early";
    }
}

Result __plug_process_launch(Launchpad *launchpad, int *pid)
{
    return task_launch(scheduler_running(), launchpad, pid);
}

Result __plug_process_sleep(int time)
{
    return task_sleep(scheduler_running(), time);
}

Result __plug_process_wait(int pid, int *exit_value)
{
    return task_wait(pid, exit_value);
}

/* ---Handles plugs --------------------------------------------------------- */

void __plug_handle_open(Handle *handle, const char *raw_path, OpenFlag flags)
{
    auto path = Path::parse(raw_path);
    auto &handles = scheduler_running()->handles();
    auto &domain = scheduler_running()->domain();

    auto result_or_handle_index = handles.open(domain, path, flags);

    handle->result = result_or_handle_index.result();

    if (result_or_handle_index.success())
    {
        handle->id = result_or_handle_index.unwrap();
    }
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id != HANDLE_INVALID_ID)
    {
        auto &handles = scheduler_running()->handles();

        handles.close(handle->id);
    }
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    auto &handles = scheduler_running()->handles();

    auto result_or_read = handles.read(handle->id, buffer, size);

    handle->result = result_or_read.result();

    if (result_or_read.success())
    {
        return result_or_read.unwrap();
    }
    else
    {
        return 0;
    }
}

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size)
{
    if (handle->id == INTERNAL_LOG_STREAM_HANDLE)
    {
        handle->result = SUCCESS;

        early_console_write(buffer, size);
        arch_debug_write(buffer, size);

        return size;
    }
    else
    {
        auto &handles = scheduler_running()->handles();

        auto result_or_write = handles.write(handle->id, buffer, size);

        handle->result = result_or_write.result();

        if (result_or_write.success())
        {
            return result_or_write.unwrap();
        }
        else
        {
            return 0;
        }
    }
}

Result __plug_handle_call(Handle *handle, IOCall request, void *args)
{
    UNUSED(handle);
    UNUSED(request);
    UNUSED(args);

    ASSERT_NOT_REACHED();
}

int __plug_handle_seek(Handle *handle, IO::SeekFrom from)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    auto &handles = scheduler_running()->handles();

    handle->result = handles.seek(handle->id, from).result();

    return 0;
}

int __plug_handle_tell(Handle *handle)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    auto &handles = scheduler_running()->handles();

    auto result_or_offset = handles.seek(handle->id, IO::SeekFrom::current(0));

    handle->result = result_or_offset.result();

    if (result_or_offset.success())
    {
        return result_or_offset.unwrap();
    }
    else
    {
        return 0;
    }
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    auto &handles = scheduler_running()->handles();

    handle->result = handles.stat(handle->id, stat);

    return 0;
}

// The following functions are stubbed on purpose.
// The kernel is not supposed to connect to services
// running in userspace using libsystem.

void __plug_process_exit(int)
{
    ASSERT_NOT_REACHED();
}

Result __plug_process_cancel(int) { ASSERT_NOT_REACHED(); }

Result __plug_process_get_directory(char *, size_t) { ASSERT_NOT_REACHED(); }

Result __plug_process_set_directory(const char *) { ASSERT_NOT_REACHED(); }

String __plug_process_resolve(String raw_path) { return raw_path; }

void __plug_handle_connect(Handle *, const char *) { ASSERT_NOT_REACHED(); }

void __plug_handle_accept(Handle *, Handle *) { ASSERT_NOT_REACHED(); }

Result __plug_create_pipe(int *, int *) { ASSERT_NOT_REACHED(); }

Result __plug_create_term(int *, int *) { ASSERT_NOT_REACHED(); }
