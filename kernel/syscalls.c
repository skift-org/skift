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

#include <skift/atomic.h>
#include <skift/cstring.h>
#include <skift/logger.h>
#include <skift/system.h>

#include <hjert/shared/syscalls.h>

#include <hjert/filesystem.h>
#include <hjert/memory.h>
#include <hjert/serial.h>
#include <hjert/syscalls.h>
#include <hjert/tasking.h>

typedef int (*syscall_handler_t)(int, int, int, int, int);

int sys_not_implemented()
{
    logger_log(LOG_WARNING, "Not implemented syscall!");
    return 0;
}

/* --- Process -------------------------------------------------------------- */

int sys_process_this(void)
{
    return sheduler_running_id();
}

int sys_process_exec(const char *file_name, const char **argv)
{
    return task_exec(file_name, argv);
}

int sys_process_exit(int code)
{
    task_exit(code);
    return 0;
}

int sys_process_cancel(int pid)
{
    int result;
    
    ATOMIC({
        result = task_cancel(task_getbyid(pid), -1);
    });

    return result;
}

int sys_process_map(uint addr, uint count)
{
    return task_memory_map(sheduler_running(), addr, count);
}

int sys_process_unmap(uint addr, uint count)
{
    return task_memory_unmap(sheduler_running(), addr, count);
}

int sys_process_alloc(uint count)
{
    return task_memory_alloc(sheduler_running(), count);
}

int sys_process_free(uint addr, uint count)
{
    task_memory_free(sheduler_running(), addr, count);
    return 0;
}

int sys_process_get_cwd(char* buffer, uint size)
{
    task_get_cwd(sheduler_running(), buffer, size);
    return 0;
}

int sys_process_set_cwd(const char* path)
{
    return task_set_cwd(sheduler_running(), path);
}

int sys_process_sleep(int time)
{
    task_sleep(time);
    return 0;
}

int sys_process_wakeup(TASK t)
{
    int result;
    
    ATOMIC({
        result = task_wakeup(task_getbyid(t));
    });

    return result; 
}

int sys_process_wait(TASK t, int *exitvalue)
{
    return task_wait(t, exitvalue);
}

/* --- Messaging ------------------------------------------------------------ */
int sys_messaging_send(TASK to, const char *name, void *payload, uint size, uint flags)
{
    int result;
    
    ATOMIC({
        result = messaging_send(task_getbyid(to), name, payload, size, flags);
    });

    return result; 
}

int sys_messaging_broadcast(const char *channel, const char *name, void *payload, uint size, uint flags)
{
    return messaging_broadcast(channel, name, payload, size, flags);
}

int sys_messaging_receive(message_t *msg, int wait)
{
    // FIXME: why "&& true" !?
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
    return task_open_file(sheduler_running(), path, flags);
}

int sys_filesystem_close(int fd)
{
    return task_close_file(sheduler_running(), fd);
}

int sys_filesystem_read(int fd, void *buffer, uint size)
{
    return task_read_file(sheduler_running(), fd, buffer, size);
}

int sys_filesystem_write(int fd, void *buffer, uint size)
{
    return task_write_file(sheduler_running(), fd, buffer, size);
}

int sys_filesystem_ioctl(int fd, int request, void *args)
{
    return task_ioctl_file(sheduler_running(), fd, request, args);
}

int sys_filesystem_seek(int fd, int offset, iostream_whence_t whence)
{
    return task_seek_file(sheduler_running(), fd, offset, whence);
}

int sys_filesystem_tell(int fd, iostream_whence_t whence)
{
    return task_tell_file(sheduler_running(), fd, whence);
}

int sys_filesystem_fstat(int fd, iostream_stat_t *stat)
{
    return task_fstat_file(sheduler_running(), fd, stat);
}

int sys_filesystem_mkdir(const char *dir_path)
{
    path_t *p = task_cwd_resolve(sheduler_running(), dir_path);

    int result = filesystem_mkdir(ROOT, p);
    
    path_delete(p);
    
    return result;
}

int sys_filesystem_link(const char* old_path, const char* new_path)
{
    path_t *oldp = task_cwd_resolve(sheduler_running(), old_path);
    path_t *newp = task_cwd_resolve(sheduler_running(), new_path);
    
    int result = filesystem_link(ROOT, oldp, ROOT, newp);

    path_delete(oldp);
    path_delete(newp);
    
    return result;
}

int sys_filesystem_unlink(const char *link_path)
{
    path_t *p = task_cwd_resolve(sheduler_running(),link_path);
    
    int result = filesystem_unlink(ROOT, p);
    
    path_delete(p);
    
    return result;
}

int sys_filesystem_rename(const char *old_path, const char* new_path)
{
    path_t* oldp = task_cwd_resolve(sheduler_running(), old_path);
    path_t* newp = task_cwd_resolve(sheduler_running(), new_path);

    int result = filesystem_rename(NULL, oldp, NULL, newp);

    path_delete(oldp);
    path_delete(newp);

    return result;
}

/* --- Sytem info getter ---------------------------------------------------- */

int sys_system_get_info(system_info_t* info)
{
    strlcpy(info->kernel_name, __kernel_name, SYSTEM_INFO_FIELD_SIZE);
    
    snprintf(info->kernel_release, SYSTEM_INFO_FIELD_SIZE, 
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

    status->running_tasks = task_count();

    return 0;
}

static int (*syscalls[])() =
    {
        [SYS_PROCESS_SELF] = sys_process_this,
        [SYS_PROCESS_EXEC] = sys_process_exec,
        [SYS_PROCESS_SPAWN] = sys_not_implemented,
        [SYS_PROCESS_EXIT] = sys_process_exit,
        [SYS_PROCESS_CANCEL] = sys_process_cancel,
        [SYS_PROCESS_MAP] = sys_process_map,
        [SYS_PROCESS_UNMAP] = sys_process_unmap,
        [SYS_PROCESS_ALLOC] = sys_process_alloc,
        [SYS_PROCESS_FREE] = sys_process_free,
        [SYS_PROCESS_GET_CWD] = sys_process_get_cwd,
        [SYS_PROCESS_SET_CWD] = sys_process_set_cwd,
        [SYS_PROCESS_SLEEP] = sys_process_sleep,
        [SYS_PROCESS_WAKEUP] = sys_process_wakeup,
        [SYS_PROCESS_WAIT] = sys_process_wait,

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
        [SYS_FILESYSTEM_RENAME] = sys_filesystem_rename,

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
        logger_log(LOG_SEVERE, "Unknow syscall ID=%d call by PROCESS=%d.", syscall, sheduler_running_id());
        logger_log(LOG_INFO, "EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);
        context->eax = 0;
    }
}