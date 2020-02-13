#pragma once

#include "kernel/tasking.h"

error_t task_fshandle_open(Task *task, int *handle_index, const char *path, OpenFlag flags);

error_t task_fshandle_close(Task *task, int handle_index);

void task_fshandle_close_all(Task *task);

error_t task_fshandle_select(Task *task, int *handle_indices, SelectEvent *events, size_t count, int *selected_index, SelectEvent *selected_events);

error_t task_fshandle_read(Task *task, int handle_index, void *buffer, size_t size, size_t *readed);

error_t task_fshandle_write(Task *task, int handle_index, const void *buffer, size_t size, size_t *written);

error_t task_fshandle_seek(Task *task, int handle_index, int offset, Whence whence);

error_t task_fshandle_tell(Task *task, int handle_index, Whence whence, int *offset);

error_t task_fshandle_call(Task *task, int handle_index, int request, void *args);

error_t task_fshandle_stat(Task *task, int handle_index, FileState *stat);

error_t task_fshandle_connect(Task *task, int *handle_index, const char *path);

error_t task_fshandle_accept(Task *task, int handle_index, int *connection_handle_index);

error_t task_fshandle_send(Task *task, int handle_index, Message *message);

error_t task_fshandle_receive(Task *task, int handle_index, Message *message);

error_t task_fshandle_payload(Task *task, int handle_index, Message *message);

error_t task_fshandle_discard(Task *task, int handle_index);

error_t task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index);

error_t task_create_term(Task *task, int *master_handle_index, int *slave_handle_index);