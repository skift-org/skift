#pragma once

#include "kernel/tasking/Task.h"

ResultOr<int> task_fshandle_open(Task *task, Path &path, OpenFlag flags);

Result task_fshandle_close(Task *task, int handle_index);

void task_fshandle_close_all(Task *task);

Result task_fshandle_reopen(Task *task, int handle, int *reopened);

Result task_fshandle_copy(Task *task, int source, int destination);

Result task_fshandle_poll(Task *task, HandleSet *handles_set, int *selected_index, PollEvent *selected_events, Timeout timeout);

ResultOr<size_t> task_fshandle_read(Task *task, int handle_index, void *buffer, size_t size);

ResultOr<size_t> task_fshandle_write(Task *task, int handle_index, const void *buffer, size_t size);

ResultOr<int> task_fshandle_seek(Task *task, int handle_index, int offset, Whence whence);

Result task_fshandle_call(Task *task, int handle_index, IOCall request, void *args);

Result task_fshandle_stat(Task *task, int handle_index, FileState *stat);

ResultOr<int> task_fshandle_connect(Task *task, Path &socket_path);

ResultOr<int> task_fshandle_accept(Task *task, int socket_handle_index);

Result task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index);

Result task_create_term(Task *task, int *server_handle_index, int *client_handle_index);
