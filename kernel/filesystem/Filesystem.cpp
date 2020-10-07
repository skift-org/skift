
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

#define ASSERT_FILESYSTEM_READY                                              \
    if (!root != nullptr)                                                    \
    {                                                                        \
        system_panic("Trying to use the filesystem before initialization."); \
    }

void filesystem_initialize()
{
    logger_info("Initializing filesystem...");

    _filesystem_root = new FsDirectory();

    logger_info("File system root at 0x%x", _filesystem_root);
}

FsNode *filesystem_find_and_ref(Path *path)
{
    assert(_filesystem_root != nullptr);

    _filesystem_root->ref();
    FsNode *current = _filesystem_root;

    for (size_t i = 0; i < path_element_count(path); i++)
    {
        if (current && current->type == FILE_TYPE_DIRECTORY)
        {
            const char *element = path_peek_at(path, i);

            current->acquire(scheduler_running_id());
            auto found = current->find(element);
            current->release(scheduler_running_id());

            current->deref();
            current = found;
        }
        else
        {
            return nullptr;
        }
    }

    return current;
}

FsNode *filesystem_find_parent_and_ref(Path *path)
{
    const char *child_name = path_pop(path);
    FsNode *parent = filesystem_find_and_ref(path);
    path_push(path, child_name);

    return parent;
}

Result filesystem_open(Path *path, OpenFlag flags, FsHandle **handle)
{

    *handle = nullptr;
    bool should_create_if_not_present = (flags & OPEN_CREATE) == OPEN_CREATE;

    FsNode *node = filesystem_find_and_ref(path);

    if (!node && should_create_if_not_present)
    {
        FsNode *parent = filesystem_find_parent_and_ref(path);

        if (parent)
        {
            if (flags & OPEN_SOCKET)
            {
                node = new FsSocket();
            }
            else
            {
                node = new FsFile();
            }

            parent->acquire(scheduler_running_id());
            parent->link(path_filename(path), node);
            parent->release(scheduler_running_id());

            parent->deref();
        }
    }

    if (!node)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if ((flags & OPEN_DIRECTORY) && node->type != FILE_TYPE_DIRECTORY)
    {
        node->deref();

        return ERR_NOT_A_DIRECTORY;
    }

    if ((flags & OPEN_SOCKET) && node->type != FILE_TYPE_SOCKET)
    {
        node->deref();

        return ERR_NOT_A_SOCKET;
    }

    bool is_node_stream = node->type == FILE_TYPE_PIPE ||
                          node->type == FILE_TYPE_REGULAR ||
                          node->type == FILE_TYPE_DEVICE ||
                          node->type == FILE_TYPE_TERMINAL;

    if ((flags & OPEN_STREAM) && !(is_node_stream))
    {
        node->deref();

        return ERR_NOT_A_STREAM;
    }

    *handle = fshandle_create(node, flags);

    node->deref();

    return SUCCESS;
}

Result filesystem_connect(Path *path, FsHandle **connection_handle)
{
    FsNode *node = filesystem_find_and_ref(path);

    if (!node)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (node->type != FILE_TYPE_SOCKET)
    {
        node->deref();
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    Result result = fshandle_connect(node, connection_handle);

    node->deref();

    return result;
}

Result filesystem_mkdir(Path *path)
{
    if (path_element_count(path) == 0)
    {
        // We are tring to create /
        return ERR_FILE_EXISTS;
    }

    auto directory = new FsDirectory();
    Result result = filesystem_link(path, directory);
    directory->deref();

    return result;
}

Result filesystem_mkpipe(Path *path)
{
    auto pipe = new FsPipe();

    Result result = filesystem_link(path, pipe);

    pipe->deref();

    return result;
}

Result filesystem_mklink(Path *old_path, Path *new_path)
{
    FsNode *child = filesystem_find_and_ref(old_path);

    if (child == nullptr)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (child->type == FILE_TYPE_DIRECTORY)
    {
        child->deref();
        return ERR_IS_A_DIRECTORY;
    }

    return filesystem_link_and_take_ref(new_path, child);
}

Result filesystem_mklink_for_tar(Path *old_path, Path *new_path)
{
    FsNode *child = filesystem_find_and_ref(old_path);

    if (child == nullptr)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    /*if (child->type == FILE_TYPE_DIRECTORY)
    {
        child->deref()
        return ERR_IS_A_DIRECTORY;
    }*/

    return filesystem_link_and_take_ref(new_path, child);
}

Result filesystem_link_cstring(const char *path, FsNode *node)
{
    Path *path_object = path_create(path);
    Result result = filesystem_link(path_object, node);
    path_destroy(path_object);

    return result;
}

Result filesystem_link(Path *path, FsNode *node)
{
    Result result = SUCCESS;

    FsNode *parent = filesystem_find_parent_and_ref(path);

    if (!parent)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (parent->type != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    parent->acquire(scheduler_running_id());
    result = parent->link(path_filename(path), node);
    parent->release(scheduler_running_id());

cleanup_and_return:
    if (parent != nullptr)
        parent->deref();

    return result;
}

Result filesystem_link_and_take_ref_cstring(const char *path, FsNode *node)
{
    Path *path_object = path_create(path);

    Result result = filesystem_link(path_object, node);
    node->deref();

    path_destroy(path_object);
    return result;
}

Result filesystem_link_and_take_ref(Path *path, FsNode *node)
{
    Result result = filesystem_link(path, node);
    node->deref();
    return result;
}

Result filesystem_unlink(Path *path)
{
    Result result = SUCCESS;

    FsNode *parent = filesystem_find_parent_and_ref(path);

    if (!parent || !path_filename(path))
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (parent->type != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    parent->acquire(scheduler_running_id());
    result = parent->unlink(path_filename(path));
    parent->release(scheduler_running_id());

cleanup_and_return:
    if (parent)
        parent->deref();

    return result;
}

Result filesystem_rename(Path *old_path, Path *new_path)
{
    // FIXME: check for loops when renaming directory

    Result result = SUCCESS;

    FsNode *child = nullptr;
    FsNode *old_parent = filesystem_find_parent_and_ref(old_path);
    FsNode *new_parent = filesystem_find_parent_and_ref(new_path);

    if (!old_parent ||
        !new_parent)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto cleanup_and_return;
    }

    if (old_parent->type != FILE_TYPE_DIRECTORY ||
        new_parent->type != FILE_TYPE_DIRECTORY)
    {
        result = ERR_NOT_A_DIRECTORY;
        goto cleanup_and_return;
    }

    new_parent->acquire(scheduler_running_id());

    if (old_parent != new_parent)
    {
        old_parent->acquire(scheduler_running_id());
    }

    child = old_parent->find(path_filename(old_path));

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

cleanup_and_return:
    if (child)
        child->deref();

    if (old_parent)
        old_parent->deref();

    if (new_parent)
        new_parent->deref();

    return result;
}
