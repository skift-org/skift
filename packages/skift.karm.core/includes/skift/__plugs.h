#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// this header list all "plugs" function between the library and the syscalls or the kernel

#include <skift/generic.h>

void __plug_init(void);

// Framework plugs to the syscalls or the kernel.
void __plug_putchar(int c);
int __plug_print(const char *buffer);

int __plug_getchar();
void __plug_read(char * buffer, uint size);

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line);

int __plug_logger_lock();
int __plug_logger_unlock();

// Memory allocator plugs
int __plug_memalloc_lock();
int __plug_memalloc_unlock();

void* __plug_memalloc_alloc(uint size);
int __plug_memalloc_free(void* memory, uint size);

// File io (TODO)
int __plug_file_open(const char* path, int flags);
int __plug_file_close();
int __plug_file_read();
int __plug_file_write();
int __plug_file_tell();
int __plug_file_seek();