/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* syscalls.c syscalls handeling code                                         */

#include <libsystem/Result.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>
#include <libsystem/system.h>

#include "kernel/clock.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/memory.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"
#include "kernel/tasking/Syscalls.h"

typedef int (*SyscallHandler)(int, int, int, int, int);

bool syscall_validate_ptr(uintptr_t ptr, size_t size)
{
    return ptr >= 0x100000 &&
           ptr + size >= 0x100000 &&
           ptr + size >= ptr;
}

/* --- Process -------------------------------------------------------------- */

int sys_process_this(void)
{
    return sheduler_running_id();
}

int sys_process_launch(Launchpad *launchpad)
{
    if (!syscall_validate_ptr((uintptr_t)launchpad, sizeof(Launchpad)))
    {
        return -ERR_BAD_ADDRESS;
    }

    return task_launch(sheduler_running(), launchpad);
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
        result = task_cancel(task_by_id(pid), -1);
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

int sys_process_get_cwd(char *buffer, uint size)
{
    task_get_cwd(sheduler_running(), buffer, size);
    return 0;
}

int sys_process_set_cwd(const char *path)
{
    return task_set_cwd(sheduler_running(), path);
}

int sys_process_sleep(int time)
{
    return task_sleep(sheduler_running(), time);
}

int sys_process_wakeup(int tid)
{
    int result;

    ATOMIC({
        result = task_wakeup(task_by_id(tid));
    });

    return result;
}

int sys_process_wait(int tid, int *exitvalue)
{
    return task_wait(tid, exitvalue);
}

/* --- Shared memory -------------------------------------------------------- */

int sys_shared_memory_alloc(size_t size, uintptr_t *out_address)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_alloc(sheduler_running(), size, out_address);
}

int sys_shared_memory_free(uintptr_t address)
{

    return task_shared_memory_free(sheduler_running(), address);
}

int sys_shared_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)) ||
        !syscall_validate_ptr((uintptr_t)out_size, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_include(sheduler_running(), handle, out_address, out_size);
}

int sys_shared_memory_get_handle(uintptr_t address, int *out_handle)
{
    if (!syscall_validate_ptr((uintptr_t)out_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_get_handle(sheduler_running(), address, out_handle);
}

/* --- Filesystem ----------------------------------------------------------- */

int sys_filesystem_mkdir(const char *dir_path)
{
    Path *path = task_cwd_resolve(sheduler_running(), dir_path);

    int result = filesystem_mkdir(path);

    path_destroy(path);

    return result;
}

int sys_filesystem_mkpipe(const char *fifo_path)
{
    Path *path = task_cwd_resolve(sheduler_running(), fifo_path);

    int result = filesystem_mkpipe(path);

    path_destroy(path);

    return result;
}

int sys_filesystem_link(const char *old_path, const char *new_path)
{
    Path *oldp = task_cwd_resolve(sheduler_running(), old_path);
    Path *newp = task_cwd_resolve(sheduler_running(), new_path);

    int result = filesystem_mklink(oldp, newp);

    path_destroy(oldp);
    path_destroy(newp);

    return result;
}

int sys_filesystem_unlink(const char *link_path)
{
    Path *path = task_cwd_resolve(sheduler_running(), link_path);

    int result = filesystem_unlink(path);

    path_destroy(path);

    return result;
}

int sys_filesystem_rename(const char *old_path, const char *new_path)
{
    Path *oldp = task_cwd_resolve(sheduler_running(), old_path);
    Path *newp = task_cwd_resolve(sheduler_running(), new_path);

    int result = filesystem_rename(oldp, newp);

    path_destroy(oldp);
    path_destroy(newp);

    return result;
}

/* --- Sytem info getter ---------------------------------------------------- */

int sys_system_get_info(SystemInfo *info)
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

    return SUCCESS;
}

int sys_system_get_status(SystemStatus *status)
{
    // FIXME: get a real uptime value;
    status->uptime = 0;

    status->total_ram = memory_get_total();
    status->used_ram = memory_get_used();

    status->running_tasks = task_count();

    return SUCCESS;
}

int sys_system_get_time(TimeStamp *timestamp)
{
    *timestamp = clock_now();

    return SUCCESS;
}

int sys_system_get_ticks()
{
    return sheduler_get_ticks();
}

/* --- Create --------------------------------------------------------------- */

int sys_create_pipe(int *reader_handle, int *writer_handle)
{
    if (!syscall_validate_ptr((uintptr_t)reader_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)writer_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_create_pipe(sheduler_running(), reader_handle, writer_handle);
}

int sys_create_term(int *master_handle, int *slave_handle)
{
    if (!syscall_validate_ptr((uintptr_t)master_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)slave_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_create_term(sheduler_running(), master_handle, slave_handle);
}

/* --- Handles -------------------------------------------------------------- */

int sys_handle_open(int *handle, const char *path, OpenFlag flags)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_open(sheduler_running(), handle, path, flags);
}

int sys_handle_close(int handle)
{
    return task_fshandle_close(sheduler_running(), handle);
}

int sys_handle_select(
    HandleSet *handles_set,
    int *selected,
    SelectEvent *selected_events,
    Timeout timeout)
{
    if (!syscall_validate_ptr((uintptr_t)handles_set, sizeof(HandleSet)) ||
        !syscall_validate_ptr((uintptr_t)handles_set->handles, sizeof(int) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)handles_set->events, sizeof(SelectEvent) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)selected, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)selected_events, sizeof(SelectEvent)))
    {
        return ERR_BAD_ADDRESS;
    }

    if (handles_set->count > PROCESS_HANDLE_COUNT)
    {
        return ERR_TOO_MANY_OPEN_FILES;
    }

    // FIXME: We need to copy these because this syscall uses task_fshandle_select
    //        who block the current thread using a blocker which does a context switch.

    int *handles_copy = calloc(handles_set->count, sizeof(int));
    memcpy(handles_copy, handles_set->handles, handles_set->count * sizeof(int));

    SelectEvent *events_copy = calloc(handles_set->count, sizeof(SelectEvent));
    memcpy(events_copy, handles_set->events, handles_set->count * sizeof(SelectEvent));

    int selected_copy;
    SelectEvent selected_event_copy;

    Result result = task_fshandle_select(
        sheduler_running(),
        &(HandleSet){handles_copy, events_copy, handles_set->count},
        &selected_copy,
        &selected_event_copy,
        timeout);

    *selected = selected_copy;
    *selected_events = selected_event_copy;

    free(handles_copy);
    free(events_copy);

    return result;
}

int sys_handle_read(int handle, char *buffer, size_t size, size_t *readed)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)readed, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_read(sheduler_running(), handle, buffer, size, readed);
}

int sys_handle_write(int handle, const char *buffer, size_t size, size_t *written)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)written, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_write(sheduler_running(), handle, buffer, size, written);
}

int sys_handle_call(int handle, int request, void *args)
{
    return task_fshandle_call(sheduler_running(), handle, request, args);
}

int sys_handle_seek(int handle, int offset, Whence whence)
{
    return task_fshandle_seek(sheduler_running(), handle, offset, whence);
}

int sys_handle_tell(int handle, Whence whence, int *offset)
{
    if (!syscall_validate_ptr((uintptr_t)offset, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_tell(sheduler_running(), handle, whence, offset);
}

int sys_handle_stat(int handle, FileState *state)
{
    if (!syscall_validate_ptr((uintptr_t)state, sizeof(FileState)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_stat(sheduler_running(), handle, state);
}

int sys_handle_connect(int *handle, const char *path)
{
    return task_fshandle_connect(sheduler_running(), handle, path);
}

int sys_handle_accept(int handle, int *connection_handle)
{
    return task_fshandle_accept(sheduler_running(), handle, connection_handle);
}

int sys_handle_send(int handle, Message *message)
{
    return task_fshandle_send(sheduler_running(), handle, message);
}

int sys_handle_receive(int handle, Message *message)
{
    return task_fshandle_receive(sheduler_running(), handle, message);
}

int sys_handle_payload(int handle, Message *message)
{
    return task_fshandle_payload(sheduler_running(), handle, message);
}

int sys_handle_discard(int handle)
{
    return task_fshandle_discard(sheduler_running(), handle);
}

static int (*syscalls[__SYSCALL_COUNT])() = {
    [SYS_PROCESS_THIS] = sys_process_this,
    [SYS_PROCESS_LAUNCH] = sys_process_launch,
    [SYS_PROCESS_EXIT] = sys_process_exit,
    [SYS_PROCESS_CANCEL] = sys_process_cancel,
    [SYS_PROCESS_SLEEP] = sys_process_sleep,
    [SYS_PROCESS_WAKEUP] = sys_process_wakeup,
    [SYS_PROCESS_WAIT] = sys_process_wait,
    [SYS_PROCESS_GET_CWD] = sys_process_get_cwd,
    [SYS_PROCESS_SET_CWD] = sys_process_set_cwd,
    [SYS_PROCESS_MAP] = sys_process_map,
    [SYS_PROCESS_UNMAP] = sys_process_unmap,
    [SYS_PROCESS_ALLOC] = sys_process_alloc,
    [SYS_PROCESS_FREE] = sys_process_free,

    [SYS_SHARED_MEMORY_ALLOC] = sys_shared_memory_alloc,
    [SYS_SHARED_MEMORY_FREE] = sys_shared_memory_free,
    [SYS_SHARED_MEMORY_INCLUDE] = sys_shared_memory_include,
    [SYS_SHARED_MEMORY_GET_HANDLE] = sys_shared_memory_get_handle,

    [SYS_FILESYSTEM_MKDIR] = sys_filesystem_mkdir,
    [SYS_FILESYSTEM_MKPIPE] = sys_filesystem_mkpipe,
    [SYS_FILESYSTEM_LINK] = sys_filesystem_link,
    [SYS_FILESYSTEM_UNLINK] = sys_filesystem_unlink,
    [SYS_FILESYSTEM_RENAME] = sys_filesystem_rename,

    [SYS_SYSTEM_GET_INFO] = sys_system_get_info,
    [SYS_SYSTEM_GET_STATUS] = sys_system_get_status,
    [SYS_SYSTEM_GET_TIME] = sys_system_get_time,
    [SYS_SYSTEM_GET_TICKS] = sys_system_get_ticks,

    [SYS_CREATE_PIPE] = sys_create_pipe,
    [SYS_CREATE_TERM] = sys_create_term,

    [SYS_HANDLE_OPEN] = sys_handle_open,
    [SYS_HANDLE_CLOSE] = sys_handle_close,
    [SYS_HANDLE_SELECT] = sys_handle_select,
    [SYS_HANDLE_READ] = sys_handle_read,
    [SYS_HANDLE_WRITE] = sys_handle_write,
    [SYS_HANDLE_CALL] = sys_handle_call,
    [SYS_HANDLE_SEEK] = sys_handle_seek,
    [SYS_HANDLE_TELL] = sys_handle_tell,
    [SYS_HANDLE_STAT] = sys_handle_stat,
    [SYS_HANDLE_CONNECT] = sys_handle_connect,
    [SYS_HANDLE_ACCEPT] = sys_handle_accept,
    [SYS_HANDLE_SEND] = sys_handle_send,
    [SYS_HANDLE_RECEIVE] = sys_handle_receive,
    [SYS_HANDLE_PAYLOAD] = sys_handle_payload,
    [SYS_HANDLE_DISCARD] = sys_handle_discard,
};

SyscallHandler syscall_get_handler(Syscall syscall)
{
    if (syscall >= 0 && syscall < __SYSCALL_COUNT)
    {
        if ((SyscallHandler)syscalls[syscall] == NULL)
        {
            logger_error("Syscall not implemented ID=%d call by PROCESS=%d.", syscall, sheduler_running_id());
        }

        return (SyscallHandler)syscalls[syscall];
    }
    else
    {
        logger_error("Unknow syscall ID=%d call by PROCESS=%d.", syscall, sheduler_running_id());
        return NULL;
    }
}

#define SYSCALL_NAMES_ENTRY(__entry) #__entry,

static const char *syscall_names[] = {SYSCALL_LIST(SYSCALL_NAMES_ENTRY)};

int task_do_syscall(Syscall syscall, int arg0, int arg1, int arg2, int arg3, int arg4)
{
    SyscallHandler handler = syscall_get_handler(syscall);

    int result = 0;

    if (handler != NULL)
    {
        result = handler(arg0, arg1, arg2, arg3, arg4);
    }
    else
    {
        logger_error("Invalid syscall: (%08x, %08x, %08x, %08x, %08x)", arg0, arg1, arg2, arg3, arg4);
        result = -ERR_FUNCTION_NOT_IMPLEMENTED;
    }

    if (((syscall >= SYS_SHARED_MEMORY_ALLOC && syscall <= SYS_SHARED_MEMORY_GET_HANDLE) ||
         syscall >= SYS_HANDLE_OPEN) &&
        (result != SUCCESS && result != TIMEOUT))
    {
        logger_warn("%s(%08x, %08x, %08x, %08x, %08x) returned %s", syscall_names[syscall], arg0, arg1, arg2, arg3, arg4, result_to_string(result));
    }
    else if (syscall < SYS_HANDLE_OPEN && (int)result < 0)
    {
        logger_warn("%s(%08x, %08x, %08x, %08x, %08x) returned %s", syscall_names[syscall], arg0, arg1, arg2, arg3, arg4, result_to_string(-result));
    }

    return result;
}