
/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <libmath/math.h>
#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/node/Directory.h"
#include "kernel/node/File.h"
#include "kernel/node/Pipe.h"
#include "kernel/node/Socket.h"
#include "kernel/scheduling/Scheduler.h"

static FsNode *_filesystem_root = NULL;

#define ASSERT_FILESYSTEM_READY                                              \
    if (!root != NULL)                                                       \
    {                                                                        \
        system_panic("Trying to use the filesystem before initialization."); \
    }

void filesystem_initialize(void)
{
    logger_info("Initializing filesystem...");

    _filesystem_root = directory_create();

    logger_info("File system root at 0x%x", _filesystem_root);
}

FsNode *filesystem_find_and_ref(Path *path)
{
    assert(_filesystem_root != NULL);

    FsNode *current = fsnode_ref(_filesystem_root);

    for (size_t i = 0; i < path_element_count(path); i++)
    {
        if (current && current->type == FILE_TYPE_DIRECTORY)
        {
            const char *element = path_peek_at(path, i);

            FsNode *found = NULL;

            if (current->find)
            {
                fsnode_acquire_lock(current, scheduler_running_id());
                found = current->find(current, element);
                fsnode_release_lock(current, scheduler_running_id());
            }

            fsnode_deref(current);
            current = found;
        }
        else
        {
            return NULL;
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
    *handle = NULL;
    bool should_create_if_not_present = (flags & OPEN_CREATE) == OPEN_CREATE;

    FsNode *node = filesystem_find_and_ref(path);

    if (!node && should_create_if_not_present)
    {
        FsNode *parent = filesystem_find_parent_and_ref(path);

        if (parent)
        {
            if (parent->link)
            {
                if (flags & OPEN_SOCKET)
                {
                    node = socket_create();
                }
                else
                {
                    node = file_create();
                }

                fsnode_acquire_lock(parent, scheduler_running_id());
                parent->link(parent, path_filename(path), node);
                fsnode_release_lock(parent, scheduler_running_id());
            }

            fsnode_deref(parent);
        }
    }

    if (!node)
    {
        logger_warn("Failled to open file %s", path_as_string(path));
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if ((flags & OPEN_DIRECTORY) && node->type != FILE_TYPE_DIRECTORY)
    {
        fsnode_deref(node);

        return ERR_NOT_A_DIRECTORY;
    }

    if ((flags & OPEN_SOCKET) && node->type != FILE_TYPE_SOCKET)
    {
        fsnode_deref(node);

        return ERR_NOT_A_SOCKET;
    }

    bool is_node_stream = node->type == FILE_TYPE_PIPE ||
                          node->type == FILE_TYPE_REGULAR ||
                          node->type == FILE_TYPE_DEVICE ||
                          node->type == FILE_TYPE_TERMINAL;

    if ((flags & OPEN_STREAM) && !(is_node_stream))
    {
        fsnode_deref(node);

        return ERR_NOT_A_STREAM;
    }

    *handle = fshandle_create(node, flags);

    fsnode_deref(node);

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
        fsnode_deref(node);
        return ERR_SOCKET_OPERATION_ON_NON_SOCKET;
    }

    Result result = fshandle_connect(node, connection_handle);

    fsnode_deref(node);

    return result;
}

Result filesystem_mkdir(Path *path)
{
    if (path_element_count(path) == 0)
    {
        // We are tring to create /
        return ERR_FILE_EXISTS;
    }

    FsNode *directory = directory_create();

    Result result = filesystem_link(path, directory);

    fsnode_deref(directory);

    return result;
}

Result filesystem_mkfile(Path *path)
{
    FsNode *file = file_create();

    Result result = filesystem_link(path, file);

    fsnode_deref(file);

    return result;
}

Result filesystem_mkpipe(Path *path)
{
    FsNode *pipe = fspipe_create();

    Result result = filesystem_link(path, pipe);

    fsnode_deref(pipe);

    return result;
}

Result filesystem_mklink(Path *old_path, Path *new_path)
{
    FsNode *child = filesystem_find_and_ref(old_path);

    if (child == NULL)
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (child->type == FILE_TYPE_DIRECTORY)
    {
        fsnode_deref(child);
        return ERR_IS_A_DIRECTORY;
    }

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

    if (!parent->link)
    {
        result = ERR_OPERATION_NOT_SUPPORTED;
        goto cleanup_and_return;
    }

    fsnode_acquire_lock(parent, scheduler_running_id());
    result = parent->link(parent, path_filename(path), node);
    fsnode_release_lock(parent, scheduler_running_id());

cleanup_and_return:
    if (parent != NULL)
        fsnode_deref(parent);

    return result;
}

Result filesystem_link_and_take_ref_cstring(const char *path, FsNode *node)
{
    Path *path_object = path_create(path);

    Result result = filesystem_link(path_object, node);
    fsnode_deref(node);

    path_destroy(path_object);
    return result;
}

Result filesystem_link_and_take_ref(Path *path, FsNode *node)
{
    Result result = filesystem_link(path, node);
    fsnode_deref(node);
    return result;
}

Result filesystem_unlink(Path *path)
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

    if (!parent->unlink)
    {
        result = ERR_OPERATION_NOT_SUPPORTED;
        goto cleanup_and_return;
    }

    fsnode_acquire_lock(parent, scheduler_running_id());
    result = parent->unlink(parent, path_filename(path));
    fsnode_release_lock(parent, scheduler_running_id());

cleanup_and_return:
    if (parent != NULL)
        fsnode_deref(parent);

    return result;
}

Result filesystem_rename(Path *old_path, Path *new_path)
{
    // FIXME: check for loops when renaming directory

    Result result = SUCCESS;

    FsNode *child = NULL;
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

    if (!old_parent->unlink ||
        !old_parent->find ||
        !new_parent->link)
    {
        result = ERR_OPERATION_NOT_SUPPORTED;
        goto cleanup_and_return;
    }

    fsnode_acquire_lock(new_parent, scheduler_running_id());

    if (old_parent != new_parent)
    {
        fsnode_acquire_lock(old_parent, scheduler_running_id());
    }

    child = old_parent->find(old_parent, path_filename(old_path));

    if (!child)
    {
        result = ERR_NO_SUCH_FILE_OR_DIRECTORY;
        goto unlock_cleanup_and_return;
    }

    result = new_parent->link(new_parent, path_filename(new_path), child);

    if (result == SUCCESS)
    {
        result = old_parent->unlink(old_parent, path_filename(old_path));
    }

unlock_cleanup_and_return:

    if (old_parent != new_parent)
    {
        fsnode_release_lock(old_parent, scheduler_running_id());
    }

    fsnode_release_lock(new_parent, scheduler_running_id());

cleanup_and_return:
    if (child)
        fsnode_deref(child);

    if (old_parent)
        fsnode_deref(old_parent);

    if (new_parent)
        fsnode_deref(new_parent);

    return result;
}
