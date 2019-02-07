#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* 
 * This header list all the "plugs" function,
 * they make the connection between the framework 
 * and the syscalls of the host kernel
 * or in the case of HJERT the kernel directly.
 */

#include <skift/generic.h>
#include <skift/io.h>

// Intialize the framework.
void __plug_init(void);

// **DEPRECATED** Print a null terminated string to the screen.
// use for logging
int __plug_print(const char *buffer);

// Framework plugs to the syscalls or the kernel.
void __plug_assert_failed(
    const char *expr,
    const char *package,
    const char *file,
    const char *function,
    int line);

// Acquired the logger lock.
int __plug_logger_lock();

// Release the logger lock.
int __plug_logger_unlock();

// Acquired the memory lock.
int __plug_memalloc_lock();

// Release the memory lock.
int __plug_memalloc_unlock();

// Allocated some page of memory.
void *__plug_memalloc_alloc(uint size);

// Free previously allocated page of memory
int __plug_memalloc_free(void *memory, uint size);

/* ========================================================================== */
/* = File & I/O                                                             = */
/* ========================================================================== */

// Open a file and return the file descriptor.
int __plug_open(const char *path, int oflags);

// Open a file relative to a opended directory and return the file descriptor.
int __plug_openat(int fd, const char *path, int oflags);

// Close a file descriptor.
int __plug_close(int fd);

// Read from oppened file descriptor.
uint __plug_read(int fd, void *buffer, uint size);

// Write to oppened file descriptor.
uint __plug_write(int fd, void *buffer, uint size);

// Read from oppened file descriptor with offset.
uint __plug_pread(int fd, void *buffer, uint size, uint offset);

// Write to oppened file descriptor with offset.
uint __plug_pwrite(int fd, void *buffer, uint size, uint offset);

// Send a io control signal *ctl* to the file. Usefull for device and tty files.
int __plug_ioctl(int fd, int ctl, void *args);

int __plug_fstat(int fd, fstat_t *stat);

// Move the write head at *offset* relative to *whence*.
int __plug_seek(int fd, int offset, int whence);

// Tell where the write head is.
int __plug_tell(int fd);

// Create a new directory
void __plug_mkdir(const char *path);

// Create a new directory relative to a opended directory;
void __plug_mkdirat(int fd, const char *path);

// Delete a directory.
void __plug_rmdir(const char *path);

// Delete a directory relative to a opended directory;
void __plug_rmdirat(int fd, const char *path);

// Delete a file.
void __plug_rm(const char *path);

// Delete a file relative to a opended directory;
void __plug_rmat(int fd, const char *path);