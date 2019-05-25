#include <skift/__plugs__.h>

#include <skift/iostream.h>
#include <skift/atomic.h>
#include <skift/assert.h>

#include <hjert/tasking.h>
#include <hjert/memory.h>
#include <hjert/serial.h>

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

void __plug_lock_assert_failed(lock_t* lock, const char *file, const char *function, int line)
{
    log(LOG_FATAL, "Kernel lock assert failed: %s in %s:%s() ln%d!", (char *)lock->name, (char *)file, (char *)function, line);
    PANIC("Kernel lock assert failed (see logs)."); 
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
    return task_open_file(sheduler_running(), file_path, flags);
}

int __plug_iostream_close(int fd)
{
    return task_close_file(sheduler_running(), fd);
}

int __plug_iostream_read(int fd, void *buffer, uint size)
{
    return task_read_file(sheduler_running(), fd, buffer, size);
}

int __plug_iostream_write(int fd, const void *buffer, uint size)
{
    return task_write_file(sheduler_running(), fd, buffer, size);
}

int __plug_iostream_ioctl(int fd, int request, void *args)
{
    return task_ioctl_file(sheduler_running(), fd, request, args);
}

int __plug_iostream_seek(int fd, int offset, iostream_whence_t whence)
{
    return task_seek_file(sheduler_running(), fd, offset, whence);
}

int __plug_iostream_tell(int fd, iostream_whence_t whence)
{
    return task_tell_file(sheduler_running(), fd, whence);
}

int __plug_iostream_fstat(int fd, iostream_stat_t *stat)
{
    return task_fstat_file(sheduler_running(), fd, stat);
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

int __plug_process_exit(int code)
{
    task_exit(code);

    PANIC("Task exit failled!");

    return 0; 
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

int __plug_process_get_cwd(char* buffer, uint size)
{
    task_get_cwd(sheduler_running(), buffer, size);
    return 0;
}

int __plug_process_set_cwd(const char* cwd)
{
    return task_set_cwd(sheduler_running(), cwd);
}

int __plug_process_sleep(int time)
{
    task_sleep(time);
    return 0;
}

int __plug_process_wakeup(int pid)
{
    int result;
    
    ATOMIC({
        result = task_wakeup(task_getbyid(pid));
    });

    return result; 
}

int __plug_process_wait(int pid, int* exit_value)
{
    return task_wait(pid, exit_value);
}