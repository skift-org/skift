/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* syscalls.c syscalls handeling code                                         */

/*
 * TODO:
 * - Check pointers from user space.
 * - File system syscalls.
 * - Shared memory syscalls.
 */

#include <skift/cstring.h>
#include <skift/logger.h>
#include <skift/system.h>
#include <skift/atomic.h>

#include "kernel/tasking.h"
#include "kernel/filesystem.h"
#include "kernel/memory.h"

#include "kernel/serial.h"

#include "kernel/shared/syscalls.h"
#include "kernel/syscalls.h"

typedef int (*syscall_handler_t)(int, int, int, int, int);

int sys_not_implemented()
{
    log(LOG_WARNING, "Not implemented syscall!");
    return 0;
}

/* --- Process -------------------------------------------------------------- */

int sys_process_self()
{
    return sheduler_running_process_id();
}

int sys_process_exec(const char *file_name, const char **argv)
{
    return process_exec(file_name, argv);
}

int sys_process_exit(int code)
{
    process_exit(code);

    return 0;
}

int sys_process_cancel(int pid)
{
    atomic_begin();
    int result = process_cancel(process_getbyid(pid), -1);
    atomic_end();

    return result;
}

int sys_process_map(uint addr, uint count)
{
    return process_memory_map(sheduler_running_process(), addr, count);
}

int sys_process_unmap(uint addr, uint count)
{
    return process_memory_unmap(sheduler_running_process(), addr, count);
}

int sys_process_alloc(uint count)
{
    return process_memory_alloc(sheduler_running_process(), count);
}

int sys_process_free(uint addr, uint count)
{
    process_memory_free(sheduler_running_process(), addr, count);
    return 0;
}

int sys_process_set_cwd(const char* path)
{
    return process_set_cwd(sheduler_running_process(), path);
}

int sys_process_get_cwd(char* buffer, uint size)
{
    process_get_cwd(sheduler_running_process(), buffer, size);
    return 0;
}

/* --- Threads -------------------------------------------------------------- */

int sys_thread_self()
{
    return sheduler_running_thread_id();
}

int sys_thread_create(thread_entry_t entry, void *args)
{
    return thread_create(sheduler_running_process(), entry, args, true);
}

int sys_thread_exit(int exitval)
{
    thread_exit(exitval);
    return 0;
}

int sys_thread_cancel(THREAD t, int exitvalue)
{
    return thread_cancel(t, exitvalue);
}

int sys_thread_sleep(int time)
{
    thread_sleep(time);
    return 0;
}

int sys_thread_wakeup(THREAD t)
{
    thread_wakeup(t);
    return 0;
}

int sys_thread_wait_thread(THREAD t, int *exitvalue)
{
    return thread_wait_thread(t, exitvalue);
}

int sys_thread_wait_process(PROCESS p, int *exitvalue)
{
    return thread_wait_process(p, exitvalue);
}

/* --- Messaging ------------------------------------------------------------ */
int sys_messaging_send(PROCESS to, const char *name, void *payload, uint size, uint flags)
{
    return messaging_send(to, name, payload, size, flags);
}

int sys_messaging_broadcast(const char *channel, const char *name, void *payload, uint size, uint flags)
{
    return messaging_broadcast(channel, name, payload, size, flags);
}

int sys_messaging_receive(message_t *msg, int wait)
{
    return messaging_receive(msg, wait && true);
}

int sys_messaging_payload(void *buffer, uint size)
{
    return messaging_payload(buffer, size);
}

int sys_messaging_subscribe(const char *channel)
{
    return messaging_subscribe(channel);
}

int sys_messaging_unsubscribe(const char *channel)
{
    return messaging_unsubscribe(channel);
}

/* --- Filesystem and IO ---------------------------------------------------- */

int sys_filesystem_open(const char *path, iostream_flag_t flags)
{
    return process_open_file(sheduler_running_process(), path, flags);
}

int sys_filesystem_close(int fd)
{
    return process_close_file(sheduler_running_process(), fd);
}

int sys_filesystem_read(int fd, void *buffer, uint size)
{
    return process_read_file(sheduler_running_process(), fd, buffer, size);
}

int sys_filesystem_write(int fd, void *buffer, uint size)
{
    return process_write_file(sheduler_running_process(), fd, buffer, size);
}

int sys_filesystem_ioctl(int fd, int request, void *args)
{
    return process_ioctl_file(sheduler_running_process(), fd, request, args);
}

int sys_filesystem_seek(int fd, int offset, iostream_whence_t whence)
{
    return process_seek_file(sheduler_running_process(), fd, offset, whence);
}

int sys_filesystem_tell(int fd, iostream_whence_t whence)
{
    return process_tell_file(sheduler_running_process(), fd, whence);
}

int sys_filesystem_fstat(int fd, iostream_stat_t *stat)
{
    return process_fstat_file(sheduler_running_process(), fd, stat);
}

int sys_filesystem_mkdir(const char *dir_path)
{
    path_t *p = path(dir_path);
    int result = filesystem_mkdir(ROOT, p);
    path_delete(p);
    return result;
}

int sys_filesystem_link(const char* old_path, const char* new_path)
{
    path_t *oldp = path(old_path);
    path_t *newp = path(new_path);
    
    int result = filesystem_link(ROOT, oldp, ROOT, newp);

    path_delete(oldp);
    path_delete(newp);
    
    return result;
}

int sys_filesystem_unlink(const char *link_path)
{
    path_t *p = path(link_path);
    int result = filesystem_unlink(ROOT, p);
    path_delete(p);
    return result;
}

int sys_system_get_info(system_info_t* info)
{
    strlcpy(info->kernel_name, __kernel_name, SYSTEM_INFO_FIELD_SIZE);
    
    sprintf(info->kernel_release, SYSTEM_INFO_FIELD_SIZE, 
            __kernel_version_format, 
            
            __kernel_version_major, 
            __kernel_version_minor, 
            __kernel_version_patch, 
            __kernel_version_codename);
    
    strlcpy(info->system_name, "skift", SYSTEM_INFO_FIELD_SIZE);

    // FIXME: this should not be hard coded.
    strlcpy(info->machine, "machine", SYSTEM_INFO_FIELD_SIZE);

    return 0;
}

int sys_system_get_status(system_status_t* status)
{
    // FIXME: get a real uptime value;
    status->uptime = 0;

    status->total_ram = memory_get_total();
    status->used_ram = memory_get_used();

    status->running_process = process_count();
    status->running_threads = thread_count();

    return 0;
}

static int (*syscalls[])() =
    {
        [SYS_PROCESS_SELF] = sys_process_self,
        [SYS_PROCESS_EXEC] = sys_process_exec,
        [SYS_PROCESS_EXIT] = sys_process_exit,
        [SYS_PROCESS_CANCEL] = sys_process_cancel,
        [SYS_PROCESS_MAP] = sys_process_map,
        [SYS_PROCESS_UNMAP] = sys_process_unmap,
        [SYS_PROCESS_ALLOC] = sys_process_alloc,
        [SYS_PROCESS_FREE] = sys_process_free,
        [SYS_PROCESS_GET_CWD] = sys_process_get_cwd,
        [SYS_PROCESS_SET_CWD] = sys_process_set_cwd,

        [SYS_THREAD_SELF] = sys_thread_self,
        [SYS_THREAD_CREATE] = sys_thread_create,
        [SYS_THREAD_EXIT] = sys_thread_exit,
        [SYS_THREAD_CANCEL] = sys_thread_cancel,
        [SYS_THREAD_SLEEP] = sys_thread_sleep,
        [SYS_THREAD_WAKEUP] = sys_thread_wakeup,
        [SYS_THREAD_WAIT_THREAD] = sys_thread_wait_thread,
        [SYS_THREAD_WAIT_PROCESS] = sys_thread_wait_process,

        [SYS_MSG_SEND] = sys_messaging_send,
        [SYS_MSG_BROADCAST] = sys_messaging_broadcast,
        [SYS_MSG_RECEIVE] = sys_messaging_receive,
        [SYS_MSG_PAYLOAD] = sys_messaging_payload,
        [SYS_MSG_SUBSCRIBE] = sys_messaging_subscribe,
        [SYS_MSG_UNSUBSCRIBE] = sys_messaging_unsubscribe,

        [SYS_FILESYSTEM_OPEN] = sys_filesystem_open,
        [SYS_FILESYSTEM_CLOSE] = sys_filesystem_close,

        [SYS_FILESYSTEM_READ] = sys_filesystem_read,
        [SYS_FILESYSTEM_WRITE] = sys_filesystem_write,
        [SYS_FILESYSTEM_IOCTL] = sys_filesystem_ioctl,
        [SYS_FILESYSTEM_SEEK] = sys_filesystem_seek,
        [SYS_FILESYSTEM_TELL] = sys_filesystem_tell,
        [SYS_FILESYSTEM_FSTAT] = sys_filesystem_fstat,
        [SYS_FILESYSTEM_MKDIR] = sys_filesystem_mkdir,
        [SYS_FILESYSTEM_LINK] = sys_filesystem_link,
        [SYS_FILESYSTEM_UNLINK] = sys_filesystem_unlink,

        [SYS_SYSTEM_GET_INFO] = sys_system_get_info,
        [SYS_SYSTEM_GET_STATUS] = sys_system_get_status,
};

void syscall_dispatcher(processor_context_t *context)
{
    syscall_t syscall = context->eax;
    if (syscall >= 0 && syscall < SYSCALL_COUNT)
    {
        syscall_handler_t syscall_handler = (syscall_handler_t)syscalls[syscall];
        context->eax = syscall_handler(context->ebx, context->ecx, context->edx, context->esi, context->edi);
    }
    else
    {
        log(LOG_SEVERE, "Unknow syscall ID=%d call by PROCESS=%d.", syscall, sheduler_running_process_id());
        log(LOG_INFO, "EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);
        context->eax = 0;
    }
}