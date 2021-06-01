#pragma once

#include <abi/Filesystem.h>
#include <abi/Handle.h>
#include <abi/IOCall.h>
#include <abi/Launchpad.h>
#include <abi/System.h>
#include <abi/Time.h>

#include <libio/Seek.h>
#include <libutils/String.h>

/* --- System --------------------------------------------------------------- */

TimeStamp __plug_system_get_time();

Tick __plug_system_get_ticks();

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this();

const char *__plug_process_name();

Result __plug_process_launch(Launchpad *launchpad, int *pid);

void NO_RETURN __plug_process_exit(int code);

Result __plug_process_cancel(int pid);

String __plug_process_resolve(String raw_path);

Result __plug_process_get_directory(char *buffer, size_t size);

Result __plug_process_set_directory(const char *directory);

Result __plug_process_sleep(int time);

Result __plug_process_wait(int pid, int *exit_value);

/* --- I/O ------------------------------------------------------------------ */

Result __plug_handle_open(Handle *handle, const char *path, HjOpenFlag flags);

void __plug_handle_close(Handle *handle);

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size);

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size);

Result __plug_handle_call(Handle *handle, IOCall request, void *args);

int __plug_handle_seek(Handle *handle, IO::SeekFrom from);

int __plug_handle_tell(Handle *handle);

int __plug_handle_stat(Handle *handle, HjStat *stat);
