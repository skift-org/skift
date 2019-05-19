#include <skift/__plugs__.h>

#include <skift/iostream.h>
#include <skift/atomic.h>
#include <skift/assert.h>

#include "kernel/tasking.h"
#include "kernel/memory.h"
#include "kernel/serial.h"

/* --- Framework initialization --------------------------------------------- */ 

iostream_t *in_stream ;
iostream_t *out_stream;
iostream_t *err_stream;
iostream_t *log_stream;

iostream_t internal_log_stream = {0};

int log_stream_write(iostream_t* stream, const void* buffer, uint size)
{
    UNUSED(stream);
    return serial_write(buffer, size);
}

void __plug_init(void)
{
    internal_log_stream.write = log_stream_write;

    in_stream  = NULL;
    out_stream = &internal_log_stream;
    err_stream = &internal_log_stream;
    log_stream = &internal_log_stream;
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    log(LOG_FATAL, "Kernel assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    PANIC("Kernel assert failed (see logs).");
}

/* --- Systeme API ---------------------------------------------------------- */

// We are the system so we doesn't need that ;)
void __plug_system_get_info(system_info_t* info)
{
    UNUSED(info);
    assert(false);
}

void __plug_system_get_status(system_status_t * status)
{
    UNUSED(status);
    assert(false);
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
    log(LOG_DEBUG, "Allocated %d pages for the kernel at %08x.", size, p);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    log(LOG_DEBUG, "Free'ed %d pages for the kernel at %08x.", size, memory);
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

int __plug_iostream_open(const char *file_path, iostream_flag_t flags)
{
    return thread_open_file(sheduler_running_thread(), file_path, flags);
}

int __plug_iostream_close(int fd)
{
    return thread_close_file(sheduler_running_thread(), fd);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return thread_read_file(sheduler_running_thread(), fd, buffer, size);
}

int __plug_iostream_write(int fd, const void *buffer, uint size)
{
    return thread_write_file(sheduler_running_thread(), fd, buffer, size);
}

int __plug_iostream_ioctl(int fd, int request, void *args)
{
    return thread_ioctl_file(sheduler_running_thread(), fd, request, args);
}

int __plug_iostream_seek(int fd, int offset, iostream_whence_t whence)
{
    return thread_seek_file(sheduler_running_thread(), fd, offset, whence);
}

int __plug_iostream_tell(int fd, iostream_whence_t whence)
{
    return thread_tell_file(sheduler_running_thread(), fd, whence);
}

int __plug_iostream_fstat(int fd, iostream_stat_t *stat)
{
    return thread_fstat_file(sheduler_running_thread(), fd, stat);
}

/* --- Threads -------------------------------------------------------------- */

int __plug_thread_this(void)
{
    return sheduler_running_thread_id();
}

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void)
{
    return sheduler_running_process_id();
}