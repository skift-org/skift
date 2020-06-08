
#include <libsystem/__plugs__.h>

#include <libsystem/Assert.h>
#include <libsystem/Atomic.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

#include "kernel/clock.h"
#include "kernel/memory/Memory.h"
#include "kernel/serial.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"

/* --- Framework initialization --------------------------------------------- */

Stream *in_stream = NULL;
Stream *out_stream = NULL;
Stream *err_stream = NULL;
Stream *log_stream = NULL;

#define INTERNAL_LOG_STREAM_HANDLE HANDLE_INVALID_ID
static Stream internal_log_stream = {};

void __plug_init(void)
{
    internal_log_stream.handle.id = INTERNAL_LOG_STREAM_HANDLE;

    in_stream = NULL;
    out_stream = &internal_log_stream;
    err_stream = &internal_log_stream;
    log_stream = &internal_log_stream;
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    logger_fatal("Assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    PANIC("Kernel assert failed (see logs).");
}

void __plug_lock_assert_failed(Lock *lock, const char *file, const char *function, int line)
{
    logger_fatal("Lock assert failed: %s in %s:%s() ln%d!", (char *)lock->name, (char *)file, (char *)function, line);
    PANIC("Kernel lock assert failed (see logs).");
}

/* --- Systeme API ---------------------------------------------------------- */

// We are the system so we doesn't need that ;)
void __plug_system_get_info(SystemInfo *info)
{
    __unused(info);
    assert(false);
}

void __plug_system_get_status(SystemStatus *status)
{
    __unused(status);
    assert(false);
}

TimeStamp __plug_system_get_time(void)
{
    return clock_now();
}

uint __plug_system_get_ticks()
{
    return scheduler_get_ticks();
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

void *__plug_memalloc_alloc(uint size)
{
    void *p = (void *)memory_alloc(memory_kpdir(), size * PAGE_SIZE, MEMORY_NONE);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    return 0;
}

/* --- Logger plugs --------------------------------------------------------- */

void __plug_logger_lock(void)
{
    atomic_begin();
}

void __plug_logger_unlock(void)
{
    atomic_end();
}

void __no_return __plug_logger_fatal(void)
{
    PANIC("Fatal error occurred (see logs)!");
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void)
{
    return scheduler_running_id();
}

Result __plug_process_launch(Launchpad *launchpad, int *pid)
{
    return task_launch(scheduler_running(), launchpad, pid);
}

void __plug_process_exit(int code)
{
    task_exit(code);

    PANIC("Task exit failled!");
}

Result __plug_process_cancel(int pid)
{
    Result result;

    ATOMIC({
        result = task_cancel(task_by_id(pid), -1);
    });

    return result;
}

int __plug_process_map(uint addr, uint count)
{
    return task_memory_map(scheduler_running(), addr, count);
}

int __plug_process_unmap(uint addr, uint count)
{
    return task_memory_unmap(scheduler_running(), addr, count);
}

uint __plug_process_alloc(uint count)
{
    return task_memory_alloc(scheduler_running(), count);
}

int __plug_process_free(uint addr, uint count)
{
    task_memory_free(scheduler_running(), addr, count);
    return 0;
}

Result __plug_process_get_cwd(char *buffer, uint size)
{
    task_get_cwd(scheduler_running(), buffer, size);
    return SUCCESS;
}

Result __plug_process_set_cwd(const char *cwd)
{
    return task_set_cwd(scheduler_running(), cwd);
}

int __plug_process_sleep(int time)
{
    return task_sleep(scheduler_running(), time);
}

int __plug_process_wait(int pid, int *exit_value)
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

        return com_write(buffer, size);
    }
    else
    {
        size_t written = 0;

        handle->result = task_fshandle_write(scheduler_running(), handle->id, buffer, size, &written);

        return written;
    }
}

Result __plug_handle_call(Handle *handle, int request, void *args)
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
