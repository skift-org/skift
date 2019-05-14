#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// this header list all "plugs" function between the library and the syscalls or the kernel

#include <skift/runtime.h>
#include <skift/iostream.h>
#include <skift/system.h>

void __plug_init(void);
void __plug_fini(int exit_code);

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line);

// Logger ------------------------------------------------------------------- //

int __plug_logger_lock(void);

int __plug_logger_unlock(void);

// Memory allocator --------------------------------------------------------- //

int __plug_memalloc_lock(void);

int __plug_memalloc_unlock(void);

void *__plug_memalloc_alloc(uint size);

int __plug_memalloc_free(void *memory, uint size);

// IO stream ---------------------------------------------------------------- //

int __plug_iostream_open(const char *path, iostream_flag_t flags);

int __plug_iostream_close(int fd);

int __plug_iostream_read(int fd, void *buffer, uint size);

int __plug_iostream_write(int fd, const void *buffer, uint size);

int __plug_iostream_ioctl(int fd, int request, void *args);

int __plug_iostream_seek(int fd, int offset, iostream_whence_t whence);

int __plug_iostream_tell(int fd, iostream_whence_t whence);

int __plug_iostream_fstat(int fd, iostream_stat_t *stat);

/* --- File system ---------------------------------------------------------- */

int __plug_filesystem_link(const char* oldpath, const char* newpath);

int __plug_filesystem_unlink(const char* path);

int __plug_filesystem_mkdir(const char* path);

/* --- System --------------------------------------------------------------- */

void __plug_system_get_info(system_info_t* info);

void __plug_system_get_status(system_status_t* status);


/* --- Threads -------------------------------------------------------------- */

int __plug_thread_this(void);

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this(void);