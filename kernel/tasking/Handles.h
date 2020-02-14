#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/tasking.h"

Result task_fshandle_open(Task *task, int *handle_index, const char *path, OpenFlag flags);

Result task_fshandle_close(Task *task, int handle_index);

void task_fshandle_close_all(Task *task);

Result task_fshandle_select(
    Task *task,
    HandleSet *handles_set,
    int *selected_index,
    SelectEvent *selected_events,
    Timeout timeout);

Result task_fshandle_read(Task *task, int handle_index, void *buffer, size_t size, size_t *readed);

Result task_fshandle_write(Task *task, int handle_index, const void *buffer, size_t size, size_t *written);

Result task_fshandle_seek(Task *task, int handle_index, int offset, Whence whence);

Result task_fshandle_tell(Task *task, int handle_index, Whence whence, int *offset);

Result task_fshandle_call(Task *task, int handle_index, int request, void *args);

Result task_fshandle_stat(Task *task, int handle_index, FileState *stat);

Result task_fshandle_connect(Task *task, int *handle_index, const char *path);

Result task_fshandle_accept(Task *task, int handle_index, int *connection_handle_index);

Result task_fshandle_send(Task *task, int handle_index, Message *message);

Result task_fshandle_receive(Task *task, int handle_index, Message *message);

Result task_fshandle_payload(Task *task, int handle_index, Message *message);

Result task_fshandle_discard(Task *task, int handle_index);

Result task_create_pipe(Task *task, int *reader_handle_index, int *writer_handle_index);

Result task_create_term(Task *task, int *master_handle_index, int *slave_handle_index);
