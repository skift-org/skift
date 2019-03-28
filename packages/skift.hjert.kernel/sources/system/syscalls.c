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

#include <skift/logger.h>

#include "kernel/tasking.h"
#include "kernel/messaging.h"
#include "kernel/shared_memory.h"
#include "kernel/filesystem.h"
#include "kernel/sheduler.h"

#include "kernel/serial.h"

#include "kernel/shared/syscalls.h"
#include "kernel/syscalls.h"

typedef int (*syscall_handler_t)(int, int, int, int, int);

int sys_not_implemented()
{
    sk_log(LOG_WARNING, "Not implemented syscall!");
    return 0;
}

/* --- Process -------------------------------------------------------------- */

int sys_process_self()
{
    return process_self();
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
    return process_cancel(pid, -1);
}

int sys_process_map(uint addr, uint count)
{
    return process_map(process_self(), addr, count);
}

int sys_process_unmap(uint addr, uint count)
{
    return process_unmap(process_self(), addr, count);
}

int sys_process_alloc(uint count)
{
    return process_alloc(count);
}

int sys_process_free(uint addr, uint count)
{
    process_free(addr, count);
    return 0;
}

/* --- Threads -------------------------------------------------------------- */

int sys_thread_self()
{
    return sheduler_running_thread_id();
}

int sys_thread_create(thread_entry_t entry, void * args)
{
    return thread_create(process_self(), entry, args, true);
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

int sys_thread_wait_thread(THREAD t, int* exitvalue)
{
    return thread_wait_thread(t, exitvalue);
}

int sys_thread_wait_process(PROCESS p, int* exitvalue)
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

/* --- System I/O ----------------------------------------------------------- */

int sys_io_print(const char *msg)
{
    printf(msg);

    return 0;
}

/* --- Filesystem and IO ---------------------------------------------------- */

// TODO file decriptor

int sys_filesystem_open(const char* file_path, fsoflags_t flags)
{
    path_t* p = path(file_path);
    int result = (int)filesystem_open(ROOT, p, flags);
    path_delete(p);
    return result;
}

int sys_filesystem_close(int fd)
{
    filesystem_close((stream_t*) fd);
    return 0;
}

int sys_filesystem_read(int fd, void* buffer, uint size)
{
    return filesystem_read((stream_t*)fd, buffer, size);
}

int sys_filesystem_write(int fd, void* buffer, uint size)
{
    return filesystem_write((stream_t*)fd, buffer, size);
}

int sys_filesystem_fstat(int fd, file_stat_t *stat)
{
    return filesystem_fstat((stream_t*)fd, stat);
}

int sys_filesystem_seek(int fd, int offset, seek_origin_t origin)
{
    return filesystem_seek((stream_t*)fd, offset, origin);
}

int sys_filesystem_tell(int fd)
{
    return filesystem_tell((stream_t*)fd);
}

int sys_filesystem_mkdir(const char *dir_path)
{
    path_t* p = path(dir_path);
    int result = filesystem_mkdir(ROOT, p);
    path_delete(p);
    return result; 
}

int sys_filesystem_unlink(const char *link_path)
{
    path_t* p = path(link_path);
    int result = filesystem_unlink(ROOT, p);
    path_delete(p);
    return result; 
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

    [SYS_IO_PRINT] = sys_io_print,
    [SYS_IO_READ] = sys_not_implemented /* NOT IMPLEMENTED */,

    [SYS_FILESYSTEM_OPEN] = sys_filesystem_open,
    [SYS_FILESYSTEM_CLOSE] = sys_filesystem_close,
    [SYS_FILESYSTEM_READ] = sys_filesystem_read,
    [SYS_FILESYSTEM_WRITE] = sys_filesystem_write,
    [SYS_FILESYSTEM_FSTAT] = sys_filesystem_fstat,
    [SYS_FILESYSTEM_SEEK] = sys_filesystem_seek,
    [SYS_FILESYSTEM_TELL] = sys_filesystem_tell,
    [SYS_FILESYSTEM_MKDIR] = sys_filesystem_mkdir,
    [SYS_FILESYSTEM_UNLINK] = sys_filesystem_unlink,
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
        sk_log(LOG_SEVERE, "Unknow syscall ID=%d call by PROCESS=%d.", syscall, process_self());
        sk_log(LOG_INFO, "EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);
        context->eax = 0;
    }
}