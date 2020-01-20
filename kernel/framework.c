/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>

#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/error.h>
#include <libsystem/iostream.h>

#include "clock.h"
#include "memory.h"
#include "serial.h"
#include "tasking.h"

/* --- Framework initialization --------------------------------------------- */

IOStream *in_stream;
IOStream *out_stream;
IOStream *err_stream;
IOStream *log_stream;

IOStream internal_log_stream = {0};

int log_stream_write(IOStream *stream, const void *buffer, uint size)
{
    __unused(stream);
    return serial_write(buffer, size);
}

void __plug_init(void)
{
    internal_log_stream.write = log_stream_write;

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

/* --- Iostream plugs ------------------------------------------------------- */

int __plug_iostream_open(const char *file_path, OpenFlag flags)
{
    return task_handle_open(sheduler_running(), file_path, flags);
}

int __plug_iostream_close(int fd)
{
    return task_handle_close(sheduler_running(), fd);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return task_fshandle_read(sheduler_running(), fd, buffer, size);
}

int __plug_iostream_write(int fd, const void *buffer, uint size)
{
    return task_fshandle_write(sheduler_running(), fd, buffer, size);
}

int __plug_iostream_call(int fd, int request, void *args)
{
    return task_fshandle_call(sheduler_running(), fd, request, args);
}

int __plug_iostream_seek(int fd, int offset, Whence whence)
{
    return task_fshandle_seek(sheduler_running(), fd, whence, offset);
}

int __plug_iostream_tell(int fd, Whence whence)
{
    return task_fshandle_tell(sheduler_running(), fd, whence);
}

int __plug_iostream_stat(int fd, FileState *stat)
{
    return task_fshandle_stat(sheduler_running(), fd, stat);
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void)
{
    return sheduler_running_id();
}

int __plug_process_exec(const char *file_name, const char **argv)
{
    return task_exec(file_name, argv);
}

// TODO: void __plug_process_spawn();

void __plug_process_exit(int code)
{
    task_exit(code);

    PANIC("Task exit failled!");
}

int __plug_process_cancel(int pid)
{
    int result;

    ATOMIC({
        result = task_cancel(task_getbyid(pid), -1);
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
        result = task_wakeup(task_getbyid(pid));
    });

    return result;
}

int __plug_process_wait(int pid, int *exit_value)
{
    return task_wait(pid, exit_value);
}

/* --- Messaging plugs ------------------------------------------------------ */

int messaging_send(message_t *event)
{
    __unused(event);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_broadcast(const char *channel, message_t *event)
{
    __unused(channel);
    __unused(event);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_request(message_t *request, message_t *result, int timeout)
{
    __unused(request);
    __unused(result);
    __unused(timeout);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_receive(message_t *message, bool wait)
{
    __unused(message);
    __unused(wait);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_respond(message_t *request, message_t *result)
{
    __unused(request);
    __unused(result);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_subscribe(const char *channel)
{
    __unused(channel);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

int messaging_unsubscribe(const char *channel)
{
    __unused(channel);

    return -ERR_FUNCTION_NOT_IMPLEMENTED;
}

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags)
{
    __unused(handle);
    __unused(path);
    __unused(flags);

    ASSERT_NOT_REACHED();
}

void __plug_handle_close(Handle *handle)
{
    __unused(handle);

    ASSERT_NOT_REACHED();
}

void __plug_handle_send(Handle *handle, Message *message)
{
    __unused(handle);
    __unused(message);

    ASSERT_NOT_REACHED();
}

void __plug_handle_receive(Handle *handle, Message *message)
{
    __unused(handle);
    __unused(message);

    ASSERT_NOT_REACHED();
}

void __plug_handle_request(Handle *handle, Message *request, Message *respond)
{
    __unused(handle);
    __unused(request);
    __unused(respond);

    ASSERT_NOT_REACHED();
}

void __plug_handle_payload(Handle *handle, Message *message)
{
    __unused(handle);
    __unused(message);

    ASSERT_NOT_REACHED();
}

void __plug_handle_discard(Handle *handle)
{
    __unused(handle);

    ASSERT_NOT_REACHED();
}

void __plug_handle_respond(Handle *handle, Message *message)
{
    __unused(handle);
    __unused(message);

    ASSERT_NOT_REACHED();
}
