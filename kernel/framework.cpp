
#include <libsystem/core/Plugs.h>

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io/Stream_internal.h>
#include <libsystem/system/System.h>
#include <libsystem/thread/Atomic.h>

#include "arch/Arch.h"
#include "arch/VirtualMemory.h"

#include "kernel/graphics/EarlyConsole.h"
#include "kernel/memory/Memory.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Task-Directory.h"
#include "kernel/tasking/Task-Handles.h"
#include "kernel/tasking/Task-Lanchpad.h"
#include "kernel/tasking/Task-Memory.h"

/* --- Framework initialization --------------------------------------------- */

Stream *in_stream = nullptr;
Stream *out_stream = nullptr;
Stream *err_stream = nullptr;
Stream *log_stream = nullptr;

#define INTERNAL_LOG_STREAM_HANDLE HANDLE_INVALID_ID
static Stream internal_log_stream = {};

void __plug_init()
{
    internal_log_stream.handle.id = INTERNAL_LOG_STREAM_HANDLE;

    in_stream = nullptr;
    out_stream = &internal_log_stream;
    err_stream = &internal_log_stream;
    log_stream = &internal_log_stream;
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    logger_fatal("Assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    ASSERT_NOT_REACHED();
}

void __plug_lock_assert_failed(Lock *lock, const char *file, const char *function, int line)
{
    logger_fatal("Lock assert failed: %s in %s:%s() ln%d!", (char *)lock->name, (char *)file, (char *)function, line);
    ASSERT_NOT_REACHED();
}

/* --- Systeme API ---------------------------------------------------------- */

// We are the system so we doesn't need that ;)
void __plug_system_get_info(SystemInfo *info)
{
    __unused(info);
    ASSERT_NOT_REACHED();
}

void __plug_system_get_status(SystemStatus *status)
{
    __unused(status);
    ASSERT_NOT_REACHED();
}

TimeStamp __plug_system_get_time()
{
    return arch_get_time();
}

uint __plug_system_get_ticks()
{
    return system_get_ticks();
}

/* --- Memory allocator plugs ----------------------------------------------- */

int __plug_memalloc_lock()
{
    atomic_begin();
    return 0;
}

int __plug_memalloc_unlock()
{
    atomic_end();
    return 0;
}

void *__plug_memalloc_alloc(size_t size)
{
    uintptr_t address = 0;
    assert(memory_alloc(arch_kernel_address_space(), size, MEMORY_CLEAR, &address) == SUCCESS);
    return (void *)address;
}

void __plug_memalloc_free(void *address, size_t size)
{
    memory_free(arch_kernel_address_space(), (MemoryRange){(uintptr_t)address, size});
}

/* --- Logger plugs --------------------------------------------------------- */

void __plug_logger_lock()
{
    atomic_begin();
}

void __plug_logger_unlock()
{
    atomic_end();
}

void __no_return __plug_logger_fatal()
{
    system_panic("Fatal error occurred (see logs)!");
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this()
{
    return scheduler_running_id();
}

Result __plug_process_launch(Launchpad *launchpad, int *pid)
{
    return task_launch(scheduler_running(), launchpad, pid);
}

void __plug_process_exit(int code)
{
    scheduler_running()->cancel(code);
    system_panic("Task exit failed!");
}

Result __plug_process_cancel(int pid)
{
    __unused(pid);
    ASSERT_NOT_REACHED();
}

Result __plug_process_get_directory(char *buffer, uint size)
{
    return task_get_directory(scheduler_running(), buffer, size);
}

Result __plug_process_set_directory(const char *directory)
{
    return task_set_directory(scheduler_running(), directory);
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

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->result = task_fshandle_open(scheduler_running(), &handle->id, path, flags);
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id != HANDLE_INVALID_ID)
    {
        task_fshandle_close(scheduler_running(), handle->id);
    }
}

Result __plug_handle_select(
    HandleSet *handles,
    int *selected,
    SelectEvent *selected_events,
    Timeout timeout)
{
    return task_fshandle_select(scheduler_running(), handles, selected, selected_events, timeout);
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    size_t read = 0;

    handle->result = task_fshandle_read(scheduler_running(), handle->id, buffer, size, &read);

    return read;
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
        size_t written = 0;

        handle->result = task_fshandle_write(scheduler_running(), handle->id, buffer, size, &written);

        return written;
    }
}

Result __plug_handle_call(Handle *handle, IOCall request, void *args)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_call(scheduler_running(), handle->id, request, args);

    return handle->result;
}

int __plug_handle_seek(Handle *handle, int offset, Whence whence)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_seek(scheduler_running(), handle->id, offset, whence);

    return 0;
}

int __plug_handle_tell(Handle *handle, Whence whence)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    int offset = 0;

    handle->result = task_fshandle_tell(scheduler_running(), handle->id, whence, &offset);

    return offset;
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_stat(scheduler_running(), handle->id, stat);

    return 0;
}

void __plug_handle_connect(Handle *handle, const char *path)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_connect(scheduler_running(), &handle->id, path);
}

void __plug_handle_accept(Handle *handle, Handle *connection_handle)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_accept(scheduler_running(), handle->id, &connection_handle->id);
}

Result __plug_create_pipe(int *reader_handle, int *writer_handle)
{
    return task_create_pipe(scheduler_running(), reader_handle, writer_handle);
}

Result __plug_create_term(int *master_handle, int *slave_handle)
{
    return task_create_term(scheduler_running(), master_handle, slave_handle);
}
