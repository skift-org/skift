
#include <libsystem/Logger.h>

#include "kernel/node/Pipe.h"
#include "kernel/node/Terminal.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Handles.h"

ResultOr<int> Handles::add(RefPtr<FsHandle> handle)
{
    LockHolder holder(_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        if (_handles[i] == nullptr)
        {
            _handles[i] = handle;

            return i;
        }
    }

    return ERR_TOO_MANY_HANDLE;
}

Result Handles::add_at(RefPtr<FsHandle> handle, int index)
{
    if (index < 0 && index >= PROCESS_HANDLE_COUNT)
    {
        return ERR_BAD_HANDLE;
    }

    LockHolder holder(_lock);
    _handles[index] = handle;
    return SUCCESS;
}

bool Handles::is_valid_handle(int handle)
{
    return handle >= 0 && handle < PROCESS_HANDLE_COUNT &&
           _handles[handle] != nullptr;
}

Result Handles::remove(int handle_index)
{
    LockHolder holder(_lock);

    if (!is_valid_handle(handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, scheduler_running_id());
        return ERR_BAD_HANDLE;
    }

    _handles[handle_index] = nullptr;

    return SUCCESS;
}

RefPtr<FsHandle> Handles::acquire(int handle_index)
{
    LockHolder holder(_lock);

    if (!is_valid_handle(handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, scheduler_running_id());
        return nullptr;
    }

    _handles[handle_index]->acquire(scheduler_running_id());
    return _handles[handle_index];
}

Result Handles::release(int handle_index)
{
    LockHolder holder(_lock);

    if (!is_valid_handle(handle_index))
    {
        logger_warn("Got a bad handle %d from task %d", handle_index, scheduler_running_id());
        return ERR_BAD_HANDLE;
    }

    _handles[handle_index]->release(scheduler_running_id());
    return SUCCESS;
}

ResultOr<int> Handles::open(Domain &domain, Path &path, OpenFlag flags)
{
    auto open_result = domain.open(path, flags);

    if (!open_result)
    {
        return open_result.result();
    }

    return add(*open_result);
}

ResultOr<int> Handles::connect(Domain &domain, Path &path)
{
    auto connect_result = domain.connect(path);

    if (!connect_result)
    {
        return connect_result.result();
    }

    return add(*connect_result);
}

Result Handles::close(int handle_index)
{
    return remove(handle_index);
}

void Handles::close_all()
{
    LockHolder holder(_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        _handles[i] = nullptr;
    }
}

Result Handles::reopen(int handle, int *reopened)
{
    auto original_handle = acquire(handle);

    if (original_handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto reopened_handle = make<FsHandle>(*original_handle);

    auto result_or_handle_index = add(reopened_handle);

    if (!result_or_handle_index.success())
    {
        return result_or_handle_index.result();
    }

    *reopened = result_or_handle_index.take_value();

    return SUCCESS;
}

Result Handles::copy(int source, int destination)
{
    auto source_handle = acquire(source);

    if (!source_handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto copy_handle = make<FsHandle>(*source_handle);

    return add_at(copy_handle, destination);
}

Result Handles::poll(
    HandleSet *handles_set,
    int *selected_index,
    PollEvent *selected_events,
    Timeout timeout)
{
    Result result = SUCCESS;
    Vector<Selected> selected;

    for (size_t i = 0; i < handles_set->count; i++)
    {
        auto handle = acquire(handles_set->handles[i]);

        if (!handle)
        {
            result = ERR_BAD_HANDLE;

            goto cleanup_and_return;
        }

        selected.push_back({handles_set->handles[i], handle, handles_set->events[i]});
    }

    {
        BlockerSelect blocker{selected};

        BlockerResult blocker_result = task_block(scheduler_running(), &blocker, timeout);

        if (blocker_result == BLOCKER_TIMEOUT)
        {
            result = TIMEOUT;
        }

        if (blocker.selected())
        {
            *selected_index = blocker.selected()->handle_index;
            *selected_events = blocker.selected()->events;
        }
    }

cleanup_and_return:
    for (size_t i = 0; i < selected.count(); i++)
    {
        release(selected[i].handle_index);
    }

    return result;
}

ResultOr<size_t> Handles::read(int handle_index, void *buffer, size_t size)
{
    auto handle = acquire(handle_index);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto result_or_read = handle->read(buffer, size);

    release(handle_index);

    return result_or_read;
}

ResultOr<size_t> Handles::write(int handle_index, const void *buffer, size_t size)
{
    auto handle = acquire(handle_index);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto result_or_written = handle->write(buffer, size);

    release(handle_index);

    return result_or_written;
}

ResultOr<int> Handles::seek(int handle_index, int offset, Whence whence)
{
    auto handle = acquire(handle_index);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto seek_result = handle->seek(offset, whence);

    release(handle_index);

    return seek_result;
}

Result Handles::call(int handle_index, IOCall request, void *args)
{
    auto handle = acquire(handle_index);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto result = handle->call(request, args);

    release(handle_index);

    return result;
}

Result Handles::stat(int handle_index, FileState *stat)
{
    auto handle = acquire(handle_index);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto result = handle->stat(stat);

    release(handle_index);

    return result;
}

ResultOr<int> Handles::accept(int socket_handle_index)
{
    auto socket_handle = acquire(socket_handle_index);

    if (socket_handle == nullptr)
    {
        return ERR_BAD_HANDLE;
    }

    auto accept_result = socket_handle->accept();

    if (!accept_result)
    {
        release(socket_handle_index);

        return accept_result.result();
    }

    auto add_result = add(*accept_result);

    release(socket_handle_index);

    return add_result;
}

Result Handles::duplex(
    RefPtr<FsNode> node,
    int *server,
    OpenFlag server_flags,
    int *client,
    OpenFlag client_flags)
{
    *server = HANDLE_INVALID_ID;
    *client = HANDLE_INVALID_ID;

    auto server_handle = make<FsHandle>(node, server_flags);
    auto client_handle = make<FsHandle>(node, client_flags);

    auto close_opened_handles = [&]() {
        if (*server != HANDLE_INVALID_ID)
        {
            remove(*server);
        }

        if (*client != HANDLE_INVALID_ID)
        {
            remove(*client);
        }
    };

    auto result_or_server = add(server_handle);

    if (!result_or_server)
    {
        close_opened_handles();
        return result_or_server.result();
    }
    else
    {
        *server = *result_or_server;
    }

    auto result_or_client = add(client_handle);

    if (!result_or_client)
    {
        close_opened_handles();
        return result_or_client.result();
    }
    else
    {
        *client = *result_or_client;
    }

    return SUCCESS;
}

Result Handles::term(int *server, int *client)
{
    return duplex(
        make<FsTerminal>(),

        server,
        OPEN_SERVER | OPEN_READ | OPEN_WRITE,

        client,
        OPEN_CLIENT | OPEN_READ | OPEN_WRITE);
}

Result Handles::pipe(int *reader, int *writer)
{
    return duplex(
        make<FsPipe>(),

        reader,
        OPEN_READ,
        writer,
        OPEN_WRITE);
}

Result Handles::pass(Handles &handles, int source, int destination)
{
    auto handle = acquire(source);

    if (!handle)
    {
        return ERR_BAD_HANDLE;
    }

    auto add_result = handles.add_at(make<FsHandle>(*handle), destination);

    release(source);

    return add_result;
}
