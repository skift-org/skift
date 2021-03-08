#include <libsystem/Logger.h>

#include "kernel/node/Directory.h"
#include "kernel/node/File.h"
#include "kernel/node/Pipe.h"
#include "kernel/node/Socket.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Domain.h"

Domain::Domain()
{
    _root = make<FsDirectory>();
}

Domain::Domain(const Domain &parent)
{
    _root = parent._root;
}

Domain::~Domain()
{
}

Domain &Domain::operator=(const Domain &other)
{
    if (this != &other)
    {
        _root = other._root;
    }

    return *this;
}

RefPtr<FsNode> Domain::find(Path path)
{
    auto current = root();

    for (size_t i = 0; i < path.length(); i++)
    {
        if (current && current->type() == FILE_TYPE_DIRECTORY)
        {
            auto element = path[i];

            current->acquire(scheduler_running_id());
            auto found = current->find(element);
            current->release(scheduler_running_id());

            current = found;
        }
        else
        {
            return nullptr;
        }
    }

    return current;
}

ResultOr<RefPtr<FsHandle>> Domain::open(Path path, OpenFlag flags)
{
    bool should_create_if_not_present = (flags & OPEN_CREATE) == OPEN_CREATE;

    auto node = find(path);

    if (!node && should_create_if_not_present)
    {
        auto parent = find(path.dirpath());

        if (parent)
        {
            if (flags & OPEN_SOCKET)
            {
                node = make<FsSocket>();
            }
            else
            {
                node = make<FsFile>();
            }

            parent->acquire(scheduler_running_id());
            parent->link(path.basename(), node);
            parent->release(scheduler_running_id());
        }
    }

    if (!node)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if ((flags & OPEN_DIRECTORY) && node->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    if ((flags & OPEN_SOCKET) && node->type() != FILE_TYPE_SOCKET)
    {
        return ERR_NOT_A_SOCKET;
    }

    bool is_node_stream = node->type() == FILE_TYPE_PIPE ||
                          node->type() == FILE_TYPE_REGULAR ||
                          node->type() == FILE_TYPE_DEVICE ||
                          node->type() == FILE_TYPE_TERMINAL;

    if ((flags & OPEN_STREAM) && !(is_node_stream))
    {
        return ERR_NOT_A_STREAM;
    }

    return make<FsHandle>(node, flags);
}

ResultOr<RefPtr<FsHandle>> Domain::connect(Path path)
{
    auto node = find(path);

    if (!node)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (node->type() != FILE_TYPE_SOCKET)
    {
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    node->acquire(scheduler_running_id());
    auto connection_or_result = node->connect();
    node->release(scheduler_running_id());

    if (!connection_or_result.success())
    {
        return connection_or_result.result();
    }

    auto connection = connection_or_result.take_value();
    auto connection_handle = make<FsHandle>(connection, OPEN_CLIENT);

    BlockerConnect blocker{connection};
    task_block(scheduler_running(), blocker, -1);

    return connection_handle;
}

Result Domain::mkdir(Path path)
{
    if (path.length() == 0)
    {
        // We are trying to create /
        return ERR_FILE_EXISTS;
    }

    return link(path, make<FsDirectory>());
}

Result Domain::mkpipe(Path path)
{
    return link(path, make<FsPipe>());
}

Result Domain::mklink(Path old_path, Path new_path)
{
    auto destination = find(old_path);

    if (!destination)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (destination->type() == FILE_TYPE_DIRECTORY)
    {
        return ERR_IS_A_DIRECTORY;
    }

    return link(new_path, destination);
}

Result Domain::link(Path path, RefPtr<FsNode> node)
{
    auto parent = find(path.dirpath());

    if (!parent)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->link(path.basename(), node);
    parent->release(scheduler_running_id());

    return result;
}

Result Domain::unlink(Path path)
{
    auto parent = find(path.dirpath());

    if (!parent || path.length() == 0)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->unlink(path.basename());
    parent->release(scheduler_running_id());

    return result;
}

// FIXME: check for loops when renaming directory
Result Domain::rename(Path old_path, Path new_path)
{
    auto old_parent = find(old_path.dirpath());
    auto new_parent = find(new_path.dirpath());

    if (!old_parent || !new_parent)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (old_parent->type() != FILE_TYPE_DIRECTORY ||
        new_parent->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    new_parent->acquire(scheduler_running_id());

    if (old_parent != new_parent)
    {
        old_parent->acquire(scheduler_running_id());
    }

    auto child = old_parent->find(old_path.basename());

    auto result = SUCCESS;

    if (child)
    {
        result = new_parent->link(new_path.basename(), child);

        if (result == SUCCESS)
        {
            result = old_parent->unlink(old_path.basename());
        }
    }
    else
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (old_parent != new_parent)
    {
        old_parent->release(scheduler_running_id());
    }

    new_parent->release(scheduler_running_id());

    return result;
}
