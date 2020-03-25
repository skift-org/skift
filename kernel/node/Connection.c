/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"

#define CONNECTION_BUFFER_SIZE 4096

void connection_accept(FsConnection *connection)
{
    connection->accepted = true;
}

bool connection_is_accepted(FsConnection *connection)
{
    return connection->accepted;
}

bool connection_can_read(FsConnection *connection, FsHandle *handle)
{
    if (fshandle_has_flag(handle, OPEN_CLIENT))
        return !ringbuffer_is_empty(connection->data_to_client);
    else
        return !ringbuffer_is_empty(connection->data_to_server);
}

Result connection_read(
    FsConnection *connection,
    FsHandle *handle,
    void *buffer,
    size_t size,
    size_t *readed)
{
    RingBuffer *data = NULL;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        data = connection->data_to_client;
    }
    else
    {
        data = connection->data_to_server;
    }

    *readed = ringbuffer_read(data, buffer, size);

    return SUCCESS;
}

Result connection_write(
    FsConnection *connection,
    FsHandle *handle,
    const void *buffer,
    size_t size,
    size_t *writen)
{
    RingBuffer *data = NULL;

    if (fshandle_has_flag(handle, OPEN_CLIENT))
    {
        data = connection->data_to_server;
    }
    else
    {
        data = connection->data_to_client;
    }

    *writen = ringbuffer_write(data, buffer, size);

    return SUCCESS;
}

void connection_FsOperationDestroy(FsConnection *connection)
{
    ringbuffer_destroy(connection->data_to_client);
    ringbuffer_destroy(connection->data_to_server);
}

FsNode *connection_create(void)
{
    FsConnection *connection = __create(FsConnection);

    fsnode_init(FSNODE(connection), FSNODE_CONNECTION);

    FSNODE(connection)->accept = (FsOperationAccept)connection_accept;
    FSNODE(connection)->is_accepted = (FsOperationIsAccepted)connection_is_accepted;
    FSNODE(connection)->can_read = (FsOperationCanRead)connection_can_read;
    FSNODE(connection)->read = (FsOperationRead)connection_read;
    FSNODE(connection)->write = (FsOperationWrite)connection_write;
    FSNODE(connection)->destroy = (FsOperationDestroy)connection_FsOperationDestroy;

    connection->data_to_client = ringbuffer_create(CONNECTION_BUFFER_SIZE);
    connection->data_to_server = ringbuffer_create(CONNECTION_BUFFER_SIZE);

    return FSNODE(connection);
}
