#include "system/Streams.h"

#include "system/node/Directory.h"
#include "system/node/File.h"
#include "system/node/Pipe.h"
#include "system/node/Socket.h"
#include "system/scheduling/Scheduler.h"
#include "system/tasking/Domain.h"

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

RefPtr<FsNode> Domain::find(IO::Path path)
{
    auto current = root();

    for (size_t i = 0; i < path.length(); i++)
    {
        if (current && current->type() == HJ_FILE_TYPE_DIRECTORY)
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

ResultOr<RefPtr<FsHandle>> Domain::open(IO::Path path, HjOpenFlag flags)
{
    bool should_create_if_not_present = (flags & HJ_OPEN_CREATE) == HJ_OPEN_CREATE;

    auto node = find(path);

    if (!node && should_create_if_not_present)
    {
        auto parent = find(path.dirpath());

        if (parent)
        {
            if (flags & HJ_OPEN_SOCKET)
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

    if ((flags & HJ_OPEN_DIRECTORY) && node->type() != HJ_FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    if ((flags & HJ_OPEN_SOCKET) && node->type() != HJ_FILE_TYPE_SOCKET)
    {
        return ERR_NOT_A_SOCKET;
    }

    bool is_node_stream = node->type() == HJ_FILE_TYPE_PIPE ||
                          node->type() == HJ_FILE_TYPE_REGULAR ||
                          node->type() == HJ_FILE_TYPE_DEVICE ||
                          node->type() == HJ_FILE_TYPE_TERMINAL;

    if ((flags & HJ_OPEN_STREAM) && !(is_node_stream))
    {
        return ERR_NOT_A_STREAM;
    }

    return make<FsHandle>(node, flags);
}

ResultOr<RefPtr<FsHandle>> Domain::connect(IO::Path path)
{
    auto node = find(path);

    if (!node)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (node->type() != HJ_FILE_TYPE_SOCKET)
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

    auto connection = connection_or_result.unwrap();
    auto connection_handle = make<FsHandle>(connection, HJ_OPEN_CLIENT);

    BlockerConnect blocker{connection};
    TRY(task_block(scheduler_running(), blocker, -1));

    return connection_handle;
}

HjResult Domain::mkdir(IO::Path path)
{
    if (path.length() == 0)
    {
        // We are trying to create /
        return ERR_FILE_EXISTS;
    }

    return link(path, make<FsDirectory>());
}

HjResult Domain::mkpipe(IO::Path path)
{
    return link(path, make<FsPipe>());
}

HjResult Domain::mklink(IO::Path old_path, IO::Path new_path)
{
    auto destination = find(old_path);

    if (!destination)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (destination->type() == HJ_FILE_TYPE_DIRECTORY)
    {
        return ERR_IS_A_DIRECTORY;
    }

    return link(new_path, destination);
}

HjResult Domain::link(IO::Path path, RefPtr<FsNode> node)
{
    auto parent = find(path.dirpath());

    if (!parent)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != HJ_FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->link(path.basename(), node);
    parent->release(scheduler_running_id());

    return result;
}

HjResult Domain::unlink(IO::Path path)
{
    auto parent = find(path.dirpath());

    if (!parent || path.length() == 0)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != HJ_FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->unlink(path.basename());
    parent->release(scheduler_running_id());

    return result;
}

// FIXME: check for loops when renaming directory
HjResult Domain::rename(IO::Path old_path, IO::Path new_path)
{
    auto old_parent = find(old_path.dirpath());
    auto new_parent = find(new_path.dirpath());

    if (!old_parent || !new_parent)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (old_parent->type() != HJ_FILE_TYPE_DIRECTORY ||
        new_parent->type() != HJ_FILE_TYPE_DIRECTORY)
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
