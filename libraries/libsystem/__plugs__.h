#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// this header list all "plugs" function between the library and the syscalls or the kernel

#include <libsystem/runtime.h>
#include <libsystem/iostream.h>
#include <libsystem/system.h>
#include <libsystem/lock.h>
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

// IO stream ---------------------------------------------------------------- //

int __plug_iostream_open(const char *path, IOStreamFlag flags);

int __plug_iostream_close(int fd);

int __plug_iostream_read(int fd, void *buffer, uint size);

int __plug_iostream_write(int fd, const void *buffer, uint size);

int __plug_iostream_call(int fd, int request, void *args);

int __plug_iostream_seek(int fd, int offset, IOStreamWhence whence);

int __plug_iostream_tell(int fd, IOStreamWhence whence);

int __plug_iostream_stat(int fd, IOStreamState *stat);

/* --- File system ---------------------------------------------------------- */

int __plug_filesystem_link(const char *oldpath, const char *newpath);

int __plug_filesystem_unlink(const char *path);

int __plug_filesystem_mkdir(const char *path);

/* --- System --------------------------------------------------------------- */

void __plug_system_get_info(system_info_t *info);

void __plug_system_get_status(system_status_t *status);

TimeStamp __plug_system_get_time(void);

uint __plug_system_get_ticks();

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void);

int __plug_process_exec(const char *file_name, const char **argv);

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
