/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>

#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/io/Stream.h>

#include "kernel/clock.h"
#include "kernel/memory.h"
#include "kernel/serial.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"

/* --- Framework initialization --------------------------------------------- */

Stream *in_stream = NULL;
Stream *out_stream = NULL;
Stream *err_stream = NULL;
Stream *log_stream = NULL;

#define INTERNAL_LOG_STREAM_HANDLE HANDLE_INVALID_ID
static Stream internal_log_stream = {.handle = {.id = INTERNAL_LOG_STREAM_HANDLE}};

void __plug_init(void)
{

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
    return sheduler_get_ticks();
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
    void *p = (void *)memory_alloc(memory_kpdir(), size, 0);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    return 0;
}

/* --- Logger plugs --------------------------------------------------------- */

int __plug_logger_lock()
{
    atomic_begin();
    return 0;
}

int __plug_logger_unlock()
{
    atomic_end();
    return 0;
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void)
{
    return sheduler_running_id();
}

int __plug_process_launch(Launchpad *launchpad)
{
    return task_launch(sheduler_running(), launchpad);
}

void __plug_process_exit(int code)
{
    task_exit(code);

    PANIC("Task exit failled!");
}

int __plug_process_cancel(int pid)
{
    int result;

    ATOMIC({
        result = task_cancel(task_by_id(pid), -1);
    });

    return result;
}

int __plug_process_map(uint addr, uint count)
{
    return task_memory_map(sheduler_running(), addr, count);
}

int __plug_process_unmap(uint addr, uint count)
{
    return task_memory_unmap(sheduler_running(), addr, count);
}

uint __plug_process_alloc(uint count)
{
    return task_memory_alloc(sheduler_running(), count);
}

int __plug_process_free(uint addr, uint count)
{
    task_memory_free(sheduler_running(), addr, count);
    return 0;
}

int __plug_process_get_cwd(char *buffer, uint size)
{
    task_get_cwd(sheduler_running(), buffer, size);
    return 0;
}

int __plug_process_set_cwd(const char *cwd)
{
    return task_set_cwd(sheduler_running(), cwd);
}

int __plug_process_sleep(int time)
{
    return task_sleep(sheduler_running(), time);
}

int __plug_process_wakeup(int pid)
{
    int result;

    ATOMIC({
        result = task_wakeup(task_by_id(pid));
    });

    return result;
}

int __plug_process_wait(int pid, int *exit_value)
{
    return task_wait(pid, exit_value);
}

/* ---Handles plugs --------------------------------------------------------- */

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    handle->result = task_fshandle_open(sheduler_running(), &handle->id, path, flags);
}

void __plug_handle_close(Handle *handle)
{
    if (handle->id != HANDLE_INVALID_ID)
    {
        task_fshandle_close(sheduler_running(), handle->id);
    }
}

Result __plug_handle_select(
    HandleSet *handles,
    int *selected,
    SelectEvent *selected_events,
    Timeout timeout)
{
    return task_fshandle_select(sheduler_running(), handles, selected, selected_events, timeout);
}

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    size_t readed = 0;

    handle->result = task_fshandle_read(sheduler_running(), handle->id, buffer, size, &readed);

    return readed;
}

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size)
{
    if (handle->id == INTERNAL_LOG_STREAM_HANDLE)
    {
        handle->result = SUCCESS;

        return serial_write(buffer, size);
    }
    else
    {
        size_t written = 0;

        handle->result = task_fshandle_write(sheduler_running(), handle->id, buffer, size, &written);

        return written;
    }
}

Result __plug_handle_call(Handle *handle, int request, void *args)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_call(sheduler_running(), handle->id, request, args);

    return handle->result;
}

int __plug_handle_seek(Handle *handle, int offset, Whence whence)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_seek(sheduler_running(), handle->id, offset, whence);

    return 0;
}

int __plug_handle_tell(Handle *handle, Whence whence)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    int offset = 0;

    handle->result = task_fshandle_tell(sheduler_running(), handle->id, whence, &offset);

    return offset;
}

int __plug_handle_stat(Handle *handle, FileState *stat)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_stat(sheduler_running(), handle->id, stat);

    return 0;
}

void __plug_handle_connect(Handle *handle, const char *path)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_connect(sheduler_running(), &handle->id, path);
}

void __plug_handle_accept(Handle *handle, Handle *connection_handle)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_accept(sheduler_running(), handle->id, &connection_handle->id);
}

void __plug_handle_send(Handle *handle, Message *message)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_send(sheduler_running(), handle->id, message);
}

void __plug_handle_receive(Handle *handle, Message *message)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_receive(sheduler_running(), handle->id, message);
}

void __plug_handle_payload(Handle *handle, Message *message)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_payload(sheduler_running(), handle->id, message);
}

void __plug_handle_discard(Handle *handle)
{
    assert(handle->id != INTERNAL_LOG_STREAM_HANDLE);

    handle->result = task_fshandle_discard(sheduler_running(), handle->id);
}

Result __plug_create_pipe(int *reader_handle, int *writer_handle)
{
    return task_create_pipe(sheduler_running(), reader_handle, writer_handle);
}

Result __plug_create_term(int *master_handle, int *slave_handle)
{
    return task_create_term(sheduler_running(), master_handle, slave_handle);
}
