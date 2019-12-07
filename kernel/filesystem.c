/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <libmath/math.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>
#include <libsystem/assert.h>

#include "tasking.h"
#include "filesystem.h"

static fsnode_t *root = NULL;
static Lock fslock;
static bool filesystem_ready = false;

#define IS_FS_READY                                                   \
    if (!filesystem_ready)                                            \
    {                                                                 \
        PANIC("Trying to use the filesystem before initialization."); \
    }

/* --- Filesystem ----------------------------------------------------------- */

void filesystem_setup()
{
    lock_init(fslock);

    root = fsnode(FSNODE_DIRECTORY);
    root->refcount++;

    filesystem_ready = true;
}

fsnode_t *filesystem_resolve(fsnode_t *at, Path *p)
{
    fsnode_t *current = (at == NULL) ? root : at;

    for (int i = 0; i < path_length(p); i++)
    {
        const char *element = path_element(p, i);

        if (current->type == FSNODE_DIRECTORY)
        {
            fsnode_t *directory = current;
            lock_acquire(directory->lock);
            fsdirectory_entry_t *entry = directory_entry(current, element);

            if (entry != NULL)
            {
                current = entry->node;
                lock_release(directory->lock);
            }
            else
            {
                lock_release(directory->lock);
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }

    return current;
}

fsnode_t *filesystem_resolve_parent(fsnode_t *at, Path *p)
{
    const char *child_name = path_pop(p);
    fsnode_t *child = filesystem_resolve(at, p);
    path_push(p, child_name);

    return child;
}

fsnode_t *filesystem_mknode(fsnode_t *at, Path *node_path, fsnode_type_t type)
{
    IS_FS_READY;

    fsnode_t *parent_node = filesystem_resolve_parent(at, node_path);

    if (parent_node == NULL || parent_node->type != FSNODE_DIRECTORY)
    {
        logger_warn("Failed to create new node, parent not found!");
        return NULL;
    }

    const char *child_name = path_filename(node_path);

    if (child_name == NULL || directory_has_entry(parent_node, child_name))
    {
        logger_warn("Failed to create new node, the file exist!");
        return NULL;
    }

    fsnode_t *child_node = fsnode(type);

    if (!directory_link(parent_node, child_node, child_name))
    {
        logger_warn("Failed to create new node, link() failed!");
        fsnode_delete(child_node);
        return NULL;
    }

    return child_node;
}

fsnode_t *filesystem_acquire(fsnode_t *at, Path *p, bool create)
{
    IS_FS_READY;

    lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(at, p);

    if (node == NULL && create)
    {
        node = filesystem_mknode(at, p, FSNODE_FILE);
    }

    if (node != NULL)
    {
        node->refcount++;
    }

    lock_release(fslock);

    return node;
}

void filesystem_release(fsnode_t *node)
{
    lock_acquire(fslock);

    node->refcount--;

    if (node->refcount == 0)
    {
        fsnode_delete(node);
    }

    lock_release(fslock);
}

void filesystem_dump_internal(fsnode_t *node, int depth)
{
    list_foreach(i, node->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;
        fsnode_t *node = entry->node;

        if (node->type != FSNODE_DIRECTORY)
        {
            printf("\t");
            for (int i = 0; i < depth; i++)
            {
                printf("| \t");
            }

            if (node->type == FSNODE_FILE)
            {
                printf("|-%s size: %dbytes\n", entry->name, node->file.size);
            }
            else
            {
                printf("|-%s\n", entry->name);
            }
        }
    }

    list_foreach(i, node->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;
        fsnode_t *node = entry->node;

        if (node->type == FSNODE_DIRECTORY)
        {
            printf("\t");
            for (int i = 0; i < depth; i++)
            {
                printf("| \t");
            }

            printf("|-%s %d childs\n", entry->name, node->directory.childs->count);
            filesystem_dump_internal(node, depth + 1);
        }
    }

    printf("\t");
    for (int i = 0; i < depth; i++)
    {
        printf("| \t");
    }
    printf("+\n");
    printf("\t");
    for (int i = 0; i < depth; i++)
    {
        printf("| \t");
    }
    printf("\n");
}

void filesystem_panic_dump(void)
{
    if (!filesystem_ready)
        return;

    printf("\n\tFile system:\n");
    filesystem_dump_internal(root, 0);
}

/* --- Filesystem Operations ------------------------------------------------ */

#define OPEN_OPTION(__opt) ((flags & __opt) && 1)
stream_t *filesystem_open(fsnode_t *at, Path *p, IOStreamFlag flags)
{
    IS_FS_READY;

    fsnode_t *node = filesystem_acquire(at, p, OPEN_OPTION(IOSTREAM_CREATE));

    if (node == NULL)
    {
        return NULL;
    }
    else
    {
        stream_t *s = stream(node, flags);

        if (node->type == FSNODE_FILE)
        {
            if (OPEN_OPTION(IOSTREAM_TRUNC))
                file_trunc(node);
        }
        else if (node->type == FSNODE_DIRECTORY)
        {
            s->direntries = directory_entries(node);
        }
        else if (node->type == FSNODE_DEVICE && node->device.open != NULL)
        {
            node->device.open(s);
        }

        return s;
    }
}

void filesystem_close(stream_t *s)
{
    IS_FS_READY;

    if (s != NULL)
    {
        fsnode_type_t type = s->node->type;

        filesystem_release(s->node);

        if (type == FSNODE_DIRECTORY)
        {
            free(s->direntries.entries);
        }
        else if (type == FSNODE_DEVICE && s->node->device.close != NULL)
        {
            s->node->device.close(s);
        }

        stream_delete(s);
    }
    else
    {
        logger_warn("Null stream passed");
    }
}

stream_t *filesystem_dup(stream_t *s)
{
    s->node->refcount++;
    stream_t *dup = stream(s->node, s->flags);
    return dup;
}

int filesystem_read(stream_t *stream, void *buffer, uint size)
{
    IS_FS_READY;

    assert(stream);

    if (buffer == NULL || size == 0)
    {
        return 0;
    }

    int result = -ERR_OPERATION_NOT_SUPPORTED;

    if (stream->flags & IOSTREAM_READ)
    {
        task_wait_stream(sheduler_running(), stream, filesystem_can_read);
        fsnode_t *node = stream->node;

        if (node->type == FSNODE_FILE)
        {
            result = file_read(stream, buffer, size);
        }
        else if (node->type == FSNODE_DEVICE)
        {
            device_t *dev = &node->device;

            if (dev->read != NULL)
            {
                result = dev->read(stream, buffer, size);
            }
        }
        else if (node->type == FSNODE_FIFO)
        {
            result = fifo_read(stream, buffer, size);
        }
        else if (node->type == FSNODE_DIRECTORY)
        {
            result = directory_read(stream, buffer, size);
        }

        lock_release(node->lock);
    }
    else
    {
        result = -ERR_OPERATION_NOT_PERMITTED;
    }

    if (result < 0)
    {
        logger_error(error_to_string(-result));
    }

    return result;
}

int filesystem_write(stream_t *stream, const void *buffer, uint size)
{
    IS_FS_READY;

    assert(stream);

    if (buffer == NULL || size == 0)
    {
        return 0;
    }

    int result = -ERR_OPERATION_NOT_SUPPORTED;

    if (stream->flags & IOSTREAM_WRITE)
    {
        task_wait_stream(sheduler_running(), stream, filesystem_can_write);
        fsnode_t *node = stream->node;

        if (node->type == FSNODE_FILE)
        {
            result = file_write(stream, buffer, size);
        }
        else if (node->type == FSNODE_DEVICE)
        {
            device_t *dev = &node->device;

            if (dev->write != NULL)
            {
                result = dev->write(stream, buffer, size);
            }
        }
        else if (node->type == FSNODE_FIFO)
        {
            result = fifo_write(stream, buffer, size);
        }

        lock_release(node->lock);
    }
    else
    {
        result = -ERR_OPERATION_NOT_PERMITTED;
    }

    if (result < 0)
    {
        logger_error(error_to_string(-result));
    }

    return result;
}

bool filesystem_can_read(stream_t *stream)
{
    assert(stream);

    fsnode_t *node = stream->node;

    if (node->type == FSNODE_FILE)
    {
        file_t *file = &node->file;
        return stream->offset <= file->size;
    }
    else if (node->type == FSNODE_FIFO)
    {
        fifo_t *fifo = &node->fifo;
        return !ringbuffer_is_empty(fifo->buffer);
    }

    return true;
}

bool filesystem_can_write(stream_t *stream)
{
    assert(stream);

    fsnode_t *node = stream->node;

    if (node->type == FSNODE_FIFO)
    {
        fifo_t *fifo = &node->fifo;
        return !ringbuffer_is_full(fifo->buffer);
    }

    return true;
}

int filesystem_call(stream_t *s, int request, void *args)
{
    IS_FS_READY;

    int result = -1;

    if (s != NULL)
    {
        if (s->node->type == FSNODE_DEVICE)
        {
            device_t *device = &s->node->device;

            if (device->call != NULL)
            {
                result = device->call(s, request, args);
            }
        }
    }
    else
    {
        logger_warn("Null stream passed");
    }

    return result;
}

int filesystem_stat(stream_t *s, IOStreamState *stat)
{
    IS_FS_READY;

    if (s != NULL)
    {
        return fsnode_stat(s->node, stat);
    }
    else
    {
        logger_warn("Null stream passed");
        return 1;
    }
}

int filesystem_seek(stream_t *s, int offset, IOStreamWhence origine)
{
    IS_FS_READY;

    if (s != NULL)
    {
        switch (origine)
        {
        case IOSTREAM_WHENCE_START:
            s->offset = offset;
            break;

        case IOSTREAM_WHENCE_HERE:
            s->offset += offset;
            break;

        case IOSTREAM_WHENCE_END:
            if (s->node->type == FSNODE_FILE)
            {
                lock_acquire(s->node->lock);
                s->offset = s->node->file.size + offset;
                lock_release(s->node->lock);
            }
            else if (s->node->type == FSNODE_DIRECTORY)
            {
                s->offset = s->direntries.count * sizeof(IOStreamDirentry) + offset;
            }
            else
            {
                // TODO: We don't support seeking for devices.
                // But this is going to be usefull for block devices.
                s->offset = offset;
            }
            break;

        default:
            break;
        }

        return -ERR_SUCCESS;
    }
    else
    {
        logger_warn("Null stream passed");
        return -1;
    }
}

int filesystem_tell(stream_t *s, IOStreamWhence whence)
{
    IS_FS_READY;

    if (s != NULL)
    {
        lock_acquire(s->node->lock);
        int offset = s->offset;
        int size = fsnode_size(s->node);
        lock_release(s->node->lock);

        if (size == -1)
        {
            return 0;
        }
        else
        {
            if (whence == IOSTREAM_WHENCE_END)
            {
                return offset - size;
            }
            else
            {
                return offset;
            }
        }
    }
    else
    {
        logger_warn("Null stream passed");
        return -1;
    }
}

int filesystem_mkdir(fsnode_t *at, Path *p)
{
    IS_FS_READY;
    lock_acquire(fslock);

    if (filesystem_mknode(at, p, FSNODE_DIRECTORY) != NULL)
    {
        lock_release(fslock);
        return 0;
    }
    else
    {
        lock_release(fslock);
        return -1;
    }
}

int filesystem_mkdev(fsnode_t *at, Path *p, device_t dev)
{
    IS_FS_READY;
    lock_acquire(fslock);

    fsnode_t *device_file = filesystem_mknode(at, p, FSNODE_DEVICE);

    if (device_file != NULL)
    {
        device_file->device = dev;
        lock_release(fslock);
        return 0;
    }
    else
    {
        lock_release(fslock);
        return -1;
    }
}

int filesystem_mkfile(fsnode_t *at, Path *p)
{
    IS_FS_READY;
    lock_acquire(fslock);
    if (filesystem_mknode(at, p, FSNODE_FILE) != NULL)
    {
        lock_release(fslock);
        return 0;
    }
    else
    {
        lock_release(fslock);
        return -1;
    }
}

int filesystem_mkfifo(fsnode_t *at, Path *p)
{
    IS_FS_READY;
    lock_acquire(fslock);
    if (filesystem_mknode(at, p, FSNODE_FIFO) != NULL)
    {
        lock_release(fslock);
        return 0;
    }
    else
    {
        lock_release(fslock);
        return -1;
    }
}

int filesystem_link(fsnode_t *file_at, Path *file_path, fsnode_t *link_at, Path *link_path)
{
    IS_FS_READY;

    int result = -1;

    lock_acquire(fslock);
    {
        fsnode_t *node = filesystem_resolve(file_at, file_path);

        if (node != NULL)
        {
            fsnode_t *parent = filesystem_resolve_parent(link_at, link_path);

            if (parent != NULL && parent->type == FSNODE_DIRECTORY)
            {
                if (directory_link(parent, node, path_filename(link_path)))
                {
                    result = 0;
                }
            }
        }
    }
    lock_release(fslock);

    return result;
}

int filesystem_unlink(fsnode_t *at, Path *link_path)
{
    IS_FS_READY;

    int result = -1;

    lock_acquire(fslock);
    {
        fsnode_t *parent = filesystem_resolve_parent(at, link_path);

        if (parent != NULL && parent->type == FSNODE_DIRECTORY)
        {
            if (directory_unlink(parent, path_filename(link_path)))
            {
                result = 0;
            }
        }
    }
    lock_release(fslock);

    return result;
}

int filesystem_rename(fsnode_t *atoldpath, Path *oldpath, fsnode_t *atnewpath, Path *newpath)
{
    IS_FS_READY;

    int result = -1;

    lock_acquire(fslock);
    {
        fsnode_t *node = filesystem_resolve(atoldpath, oldpath);
        if (node != NULL)
        {
            fsnode_t *parent = filesystem_resolve_parent(atnewpath, newpath);

            if (parent != NULL && parent->type == FSNODE_DIRECTORY)
            {
                if (directory_link(parent, node, path_filename(newpath)))
                {
                    directory_unlink(parent, path_filename(oldpath));
                    result = 0;
                }
            }
        }
    }
    lock_release(fslock);

    return result;
}

bool filesystem_exist(fsnode_t *at, Path *p)
{
    fsnode_t *node = filesystem_acquire(at, p, false);

    if (node != NULL)
    {
        filesystem_release(node);
    }

    return node != NULL;
}
