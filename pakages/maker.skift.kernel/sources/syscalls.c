/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>

#include "kernel/tasking.h"
#include "kernel/shared_syscalls.h"
#include "kernel/serial.h"
#include "kernel/graphic.h"
#include "kernel/mouse.h"

#include "kernel/syscalls.h"

typedef int (*syscall_t)(int, int, int, int, int);

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
    process_cancel(pid);
    return 0;
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
    return thread_self();
}

int sys_thread_create(thread_entry_t entry, void * args, int flags)
{
    return thread_create(process_self(), entry, args, flags | TASK_USER);
}

int sys_thread_exit(void* retval)
{
    thread_exit(retval);
    return 0;
}

int sys_thread_cancel(THREAD t)
{
    return thread_cancel(t);
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

int sys_thread_wait(THREAD t)
{
    return (int)thread_wait(t);
}

int sys_thread_waitproc(PROCESS p)
{
    return thread_waitproc(p);
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

int sys_messaging_receive(message_t *msg)
{
    return messaging_receive(msg);
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
    return sys_messaging_unsubscribe(channel);
}

/* --- System I/O ----------------------------------------------------------- */

int sys_io_print(const char *msg)
{
    printf(msg);

    return 0;
}

int sys_io_mouse_get_state(mouse_state_t *state)
{
    mouse_get_state(state);
    return 0;
}

int sys_io_mouse_set_state(mouse_state_t *state)
{
    mouse_set_state(state);
    return 0;
}

int sys_io_graphic_blit(unsigned int *buffer)
{
    graphic_blit(buffer);
    return 0;
}

int sys_io_graphic_blit_region(unsigned int *buffer, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    graphic_blit_region(buffer, x, y, w, h);
    return 0;
}

int sys_io_graphic_size(unsigned int *width, unsigned int *height)
{
    graphic_size(width, height);
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

        [SYS_THREAD_SELF] = sys_thread_self,
        [SYS_THREAD_CREATE] = sys_thread_create,
        [SYS_THREAD_EXIT] = sys_thread_exit,
        [SYS_THREAD_CANCEL] = sys_thread_cancel,
        [SYS_THREAD_SLEEP] = sys_thread_sleep,
        [SYS_THREAD_WAKEUP] = sys_thread_wakeup,
        [SYS_THREAD_WAIT] = sys_thread_wait,
        [SYS_THREAD_WAITPROC] = sys_thread_waitproc,

        [SYS_MSG_SEND] = sys_messaging_send,
        [SYS_MSG_BROADCAST] = sys_messaging_broadcast,
        [SYS_MSG_RECEIVE] = sys_messaging_receive,
        [SYS_MSG_PAYLOAD] = sys_messaging_payload,
        [SYS_MSG_SUBSCRIBE] = sys_messaging_subscribe,
        [SYS_MSG_UNSUBSCRIBE] = sys_messaging_unsubscribe,

        [SYS_IO_PRINT] = sys_io_print,
        [SYS_IO_READ] = sys_not_implemented /* NOT IMPLEMENTED */,

        [SYS_IO_MOUSE_GET_STATE] = sys_io_mouse_get_state,
        [SYS_IO_MOUSE_SET_STATE] = sys_io_mouse_set_state,

        [SYS_IO_GRAPHIC_BLIT] = sys_io_graphic_blit,
        [SYS_IO_GRAPHIC_BLIT_REGION] = sys_io_graphic_blit_region,
        [SYS_IO_GRAPHIC_SIZE] = sys_io_graphic_size,

        [SYS_FILE_CREATE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_DELETE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_EXISTE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_COPY] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_MOVE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_STAT] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_OPEN] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_CLOSE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_READ] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_WRITE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_FILE_IOCTL] = sys_not_implemented /* NOT IMPLEMENTED */,

        [SYS_DIR_CREATE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_DELETE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_EXISTE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_OPEN] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_CLOSE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_LISTFILE] = sys_not_implemented /* NOT IMPLEMENTED */,
        [SYS_DIR_LISTDIR] = sys_not_implemented /* NOT IMPLEMENTED */,
};

void syscall_dispatcher(context_t *context)
{
    int syscall_number = context->eax;
    if (syscall_number >= 0 && syscall_number < SYSCALL_COUNT)
    {
        syscall_t syscall = (syscall_t)syscalls[syscall_number];
        context->eax = syscall(context->ebx, context->ecx, context->edx, context->esi, context->edi);
    }
    else
    {
        sk_log(LOG_SEVERE, "Unknow syscall ID=%d call by PROCESS=%d.", syscall_number, process_self());
        sk_log(LOG_INFO, "EBX=%d, ECX=%d, EDX=%d, ESI=%d, EDI=%d", context->eax, context->ebx, context->ecx, context->edx, context->esi, context->edi);
        context->eax = 0;
    }
}