#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// this header list all "plugs" function between the library and the syscalls or the kernel

#include <abi/Filesystem.h>
#include <abi/Handle.h>
#include <abi/Launchpad.h>
#include <abi/Message.h>

#include <libsystem/lock.h>
#include <libsystem/runtime.h>
#include <libsystem/system.h>
#include <libsystem/time.h>

void __plug_init(void);

void __plug_fini(int exit_code);

void __attribute__((noreturn)) __plug_assert_failed(const char *expr, const char *file, const char *function, int line);

void __plug_lock_assert_failed(Lock *lock, const char *file, const char *function, int line);

// Logger ------------------------------------------------------------------- //

int __plug_logger_lock(void);

int __plug_logger_unlock(void);

// Memory allocator --------------------------------------------------------- //

int __plug_memalloc_lock(void);

int __plug_memalloc_unlock(void);

void *__plug_memalloc_alloc(uint size);

int __plug_memalloc_free(void *memory, uint size);

/* --- File system ---------------------------------------------------------- */

int __plug_filesystem_link(const char *oldpath, const char *newpath);

int __plug_filesystem_unlink(const char *path);

int __plug_filesystem_mkdir(const char *path);

/* --- System --------------------------------------------------------------- */

void __plug_system_get_info(SystemInfo *info);

void __plug_system_get_status(SystemStatus *status);

TimeStamp __plug_system_get_time(void);

uint __plug_system_get_ticks();

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void);

int __plug_process_launch(Launchpad *launchpad);

void __attribute__((noreturn)) __plug_process_exit(int code);

int __plug_process_cancel(int pid);

int __plug_process_map(uint addr, uint count);

int __plug_process_unmap(uint addr, uint count);

uint __plug_process_alloc(uint count);

int __plug_process_free(uint addr, uint count);

int __plug_process_get_cwd(char *buffer, uint size);

int __plug_process_set_cwd(const char *cwd);

int __plug_process_sleep(int time);

int __plug_process_wakeup(int pid);

int __plug_process_wait(int pid, int *exit_value);

/* --- new handle api ------------------------------------------------------- */

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags);
void __plug_handle_close(Handle *handle);

Result __plug_handle_select(
    HandleSet *handles,
    int *selected,
    SelectEvent *selected_events,
    Timeout timeout);

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size);
size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size);

Result __plug_handle_call(Handle *handle, int request, void *args);
int __plug_handle_seek(Handle *handle, int offset, Whence whence);

int __plug_handle_tell(Handle *handle, Whence whence);
int __plug_handle_stat(Handle *handle, FileState *stat);

void __plug_handle_connect(Handle *handle, const char *path);
void __plug_handle_accept(Handle *handle, Handle *connection_handle);

void __plug_handle_send(Handle *handle, Message *message);
void __plug_handle_receive(Handle *handle, Message *message);
void __plug_handle_payload(Handle *handle, Message *message);
void __plug_handle_discard(Handle *handle);

Result __plug_create_pipe(int *reader_handle, int *writer_handle);
Result __plug_create_term(int *master_handle, int *slave_handle);
