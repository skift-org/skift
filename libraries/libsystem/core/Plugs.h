#pragma once

#include <abi/Filesystem.h>
#include <abi/Handle.h>
#include <abi/IOCall.h>
#include <abi/Launchpad.h>
#include <abi/System.h>
#include <skift/Time.h>

#include <libutils/String.h>

extern "C" void __plug_initialize();

extern "C" void __plug_uninitialize(int exit_code);

class Lock;
__no_return void __plug_lock_ensure_failed(Lock &, const char *raison, __SOURCE_LOCATION__ location);

/* --- Logger --------------------------------------------------------------- */

void __plug_logger_lock();

void __plug_logger_unlock();

void __no_return __plug_logger_fatal();

/* --- File system ---------------------------------------------------------- */

Result __plug_filesystem_link(const char *oldpath, const char *newpath);

Result __plug_filesystem_unlink(const char *path);

Result __plug_filesystem_mkdir(const char *path);

/* --- System --------------------------------------------------------------- */

TimeStamp __plug_system_get_time();

Tick __plug_system_get_ticks();

/* --- Processes ------------------------------------------------------------ */

int __plug_process_this();

const char *__plug_process_name();

Result __plug_process_launch(Launchpad *launchpad, int *pid);

void __no_return __plug_process_exit(int code);

Result __plug_process_cancel(int pid);

String __plug_process_resolve(String raw_path);

Result __plug_process_get_directory(char *buffer, size_t size);

Result __plug_process_set_directory(const char *directory);

Result __plug_process_sleep(int time);

Result __plug_process_wait(int pid, int *exit_value);

/* --- I/O ------------------------------------------------------------------ */

void __plug_handle_open(Handle *handle, const char *path, OpenFlag flags);

void __plug_handle_close(Handle *handle);

Result __plug_handle_poll(
    HandleSet *handles,
    int *selected,
    PollEvent *selected_events,
    Timeout timeout);

size_t __plug_handle_read(Handle *handle, void *buffer, size_t size);

size_t __plug_handle_write(Handle *handle, const void *buffer, size_t size);

Result __plug_handle_call(Handle *handle, IOCall request, void *args);

int __plug_handle_seek(Handle *handle, int offset, Whence whence);

int __plug_handle_tell(Handle *handle);

int __plug_handle_stat(Handle *handle, FileState *stat);

void __plug_handle_connect(Handle *handle, const char *path);

void __plug_handle_accept(Handle *handle, Handle *connection_handle);

Result __plug_create_pipe(int *reader_handle, int *writer_handle);

Result __plug_create_term(int *server_handle, int *client_handle);
