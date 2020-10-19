
/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/Math.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/Directory.h"
#include "kernel/node/File.h"
#include "kernel/node/Pipe.h"
#include "kernel/node/Socket.h"
#include "kernel/scheduling/Scheduler.h"

static FsNode *_filesystem_root = nullptr;

static RefPtr<FsNode> filesystem_root()
{
    assert(_filesystem_root);
    return {*_filesystem_root};
}

void filesystem_initialize()
{
    logger_info("Initializing filesystem...");

    _filesystem_root = new FsDirectory();
    _filesystem_root->ref();

    logger_info("File system root at 0x%x", _filesystem_root);
}

RefPtr<FsNode> filesystem_find_and_ref(Path *path)
{
    auto current = filesystem_root();

    for (size_t i = 0; i < path_element_count(path); i++)
    {
        if (current && current->type() == FILE_TYPE_DIRECTORY)
        {
            const char *element = path_peek_at(path, i);

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

RefPtr<FsNode> filesystem_find_parent_and_ref(Path *path)
{
    const char *child_name = path_pop(path);
    auto parent = filesystem_find_and_ref(path);
    path_push(path, child_name);

    return parent;
}

ResultOr<FsHandle *> filesystem_open(Path *path, OpenFlag flags)
{
    bool should_create_if_not_present = (flags & OPEN_CREATE) == OPEN_CREATE;

    auto node = filesystem_find_and_ref(path);

    if (!node && should_create_if_not_present)
    {
        auto parent = filesystem_find_parent_and_ref(path);

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
            parent->link(path_filename(path), node);
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

    return new FsHandle(node, flags);
}

ResultOr<FsHandle *> filesystem_connect(Path *path)
{
    auto node = filesystem_find_and_ref(path);

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
    auto connection_handle = new FsHandle(connection, OPEN_CLIENT);

    task_block(scheduler_running(), new BlockerConnect(connection), -1);

    return connection_handle;
}

Result filesystem_mkdir(Path *path)
{
    if (path_element_count(path) == 0)
    {
        // We are tring to create /
        return ERR_FILE_EXISTS;
    }

    return filesystem_link(path, make<FsDirectory>());
}

Result filesystem_mkpipe(Path *path)
{
    return filesystem_link(path, make<FsPipe>());
}

Result filesystem_mklink(Path *old_path, Path *new_path)
{
    auto destination = filesystem_find_and_ref(old_path);

    if (!destination)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (destination->type() == FILE_TYPE_DIRECTORY)
    {
        return ERR_IS_A_DIRECTORY;
    }

    return filesystem_link(new_path, destination);
}

// This version allow
Result filesystem_mklink_for_tar(Path *old_path, Path *new_path)
{
    auto child = filesystem_find_and_ref(old_path);

    if (!child)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    return filesystem_link(new_path, child);
}

Result filesystem_link_cstring(const char *path, RefPtr<FsNode> node)
{
    Path *path_object = path_create(path);
    Result result = filesystem_link(path_object, node);
    path_destroy(path_object);

    return result;
}

Result filesystem_link(Path *path, RefPtr<FsNode> node)
{
    auto parent = filesystem_find_parent_and_ref(path);

    if (!parent)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->link(path_filename(path), node);
    parent->release(scheduler_running_id());

    return result;
}

Result filesystem_unlink(Path *path)
{
    auto parent = filesystem_find_parent_and_ref(path);

    if (!parent || !path_filename(path))
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (parent->type() != FILE_TYPE_DIRECTORY)
    {
        return ERR_NOT_A_DIRECTORY;
    }

    parent->acquire(scheduler_running_id());
    auto result = parent->unlink(path_filename(path));
    parent->release(scheduler_running_id());

    return result;
}

// FIXME: check for loops when renaming directory
Result filesystem_rename(Path *old_path, Path *new_path)
{
    auto old_parent = filesystem_find_parent_and_ref(old_path);
    auto new_parent = filesystem_find_parent_and_ref(new_path);

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

    auto child = old_parent->find(path_filename(old_path));

    Result result = SUCCESS;

    if (!child)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto unlock_cleanup_and_return;
    }

    result = new_parent->link(path_filename(new_path), child);

    if (result == SUCCESS)
    {
        result = old_parent->unlink(path_filename(old_path));
    }

unlock_cleanup_and_return:

    if (old_parent != new_parent)
    {
        old_parent->release(scheduler_running_id());
    }

    new_parent->release(scheduler_running_id());

    return result;
}
