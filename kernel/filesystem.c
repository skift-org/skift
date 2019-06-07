/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

/*
 * TODO:
 *  - A lot of error checking is missing
 *  - Add support for on disk file system 
 *  - Add support for .. and .
 */

#include <skift/math.h>
#include <skift/cstring.h>
#include <skift/error.h>
#include <skift/logger.h>

#include "kernel/tasking.h"
#include "kernel/filesystem.h"

static fsnode_t *root = NULL;
static lock_t fslock;
static bool filesystem_ready = false;

#define IS_FS_READY                                                   \
    if (!filesystem_ready)                                            \
    {                                                                 \
        PANIC("Trying to use the filesystem before initialization."); \
    }

/* --- Fsnode --------------------------------------------------------------- */

iostream_type_t fsnode_to_iostream_type(fsnode_type_t type)
{
    switch (type)
    {
        case FSNODE_DIRECTORY: return IOSTREAM_TYPE_DIRECTORY;
        case FSNODE_FIFO: return IOSTREAM_TYPE_FIFO;
        case FSNODE_DEVICE: return IOSTREAM_TYPE_DEVICE;
        default: return IOSTREAM_TYPE_REGULAR;
    }
}

fsnode_t *fsnode(fsnode_type_t type)
{
    fsnode_t *node = MALLOC(fsnode_t);

    node->type = type;
    node->refcount = 0;
    lock_init(node->lock);

    switch (type)
    {
    case FSNODE_FILE:
    {
        file_t *file = &node->file;

        file->buffer = malloc(512);
        file->realsize = 512;
        file->size = 0;
        break;
    }
    case FSNODE_DIRECTORY:
    {
        directory_t *dir = &node->directory;

        dir->childs = list();
        break;
    }
    case FSNODE_FIFO:
    {
        fifo_t *fifo = &node->fifo;

        lock_init(fifo->buffer_lock);
        fifo->buffer = ringbuffer(4096);

        break;
    }
    default:
        break;
    }

    return node;
}

void fsnode_delete(fsnode_t *node)
{
    logger_log(LOG_INFO, "Fsnode free: %08x", node);

    switch (node->type)
    {
    case FSNODE_FILE:
    {
        free(node->file.buffer);
        break;
    }
    case FSNODE_DIRECTORY:
    {
        list_foreach(item, node->directory.childs)
        {
            fsdirectory_entry_t *entry = item->value;
            fsnode_t *n = entry->node;

            n->refcount--;

            if (n->refcount == 0)
            {
                fsnode_delete(n);
            }
        }

        list_delete(node->directory.childs, LIST_FREE_VALUES);
        break;
    }
    case FSNODE_FIFO:
    {
        ringbuffer_delete(node->fifo.buffer);
        break;
    }
    default:
        break;
    }

    free(node);
}

int fsnode_size(fsnode_t *node)
{
    if (node->type == FSNODE_FILE)
    {
        return node->file.size;
    }

    return 0;
}

int fsnode_stat(fsnode_t *node, iostream_stat_t* stat)
{
    stat->type = fsnode_to_iostream_type(node->type);
    stat->size = fsnode_size(node);

    return 0;
}


/* --- Streams -------------------------------------------------------------- */

stream_t *stream(fsnode_t *node, iostream_flag_t flags)
{
    if (node == NULL)
        return NULL;

    stream_t *s = MALLOC(stream_t);

    s->node = node;
    s->offset = 0;
    s->flags = flags;

    return s;
}

void stream_delete(stream_t *s)
{
    free(s);
}

/* --- Files ---------------------------------------------------------------- */

void file_trunc(fsnode_t *node)
{
    lock_acquire(node->lock);

    free(node->file.buffer);

    node->file.buffer = malloc(512);
    node->file.realsize = 512;
    node->file.size = 0;

    lock_release(node->lock);
}

int file_read(stream_t *stream, void *buffer, uint size)
{
    int result = 0;

    lock_acquire(stream->node->lock);

    file_t *file = &stream->node->file;

    if (stream->offset <= file->size)
    {
        int readedsize = min(file->size - stream->offset, size);
        memcpy(buffer, (byte *)file->buffer + stream->offset, readedsize);

        result = readedsize;
        stream->offset += readedsize;
    }

    lock_release(stream->node->lock);

    return result;
}

int file_write(stream_t *stream, const void *buffer, uint size)
{
    int result = 0;

    lock_acquire(stream->node->lock);

    file_t *file = &stream->node->file;

    if (stream->flags & IOSTREAM_APPEND)
    {
        stream->offset = file->size;
    }

    if ((stream->offset + size) > file->realsize)
    {
        file->buffer = realloc(file->buffer, stream->offset + size);
        file->realsize = stream->offset + size;
    }

    file->size = max(stream->offset + size, file->size);
    memcpy((byte *)(file->buffer) + stream->offset, buffer, size);

    result = size;
    stream->offset += size;

    lock_release(stream->node->lock);

    return result;
}

/* --- Fifo ----------------------------------------------------------------- */

//FIXME: this is kind of naive

int fifo_read(stream_t *stream, void *buffer, uint size)
{
    fifo_t* fifo = &stream->node->fifo;

    lock_acquire(fifo->buffer_lock);

    int result = ringbuffer_read(fifo->buffer, buffer, size);
        
    lock_release(fifo->buffer_lock);

    return result;
}

int fifo_write(stream_t *stream, const void *buffer, uint size)
{
    fifo_t* fifo = &stream->node->fifo;

    lock_acquire(fifo->buffer_lock);
    
    int result = ringbuffer_write(fifo->buffer, buffer, size);
        
    lock_release(fifo->buffer_lock);

    return result;
}

/* --- Directories ---------------------------------------------------------- */

// only call this method if you hold the directory lock.
fsdirectory_entry_t *directory_entry(fsnode_t *dir, const char *child)
{
    list_foreach(i, dir->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;

        if (strcmp(child, entry->name) == 0)
        {
            return entry;
        }
    }

    return NULL;
}

bool directory_has_entry(fsnode_t *dir, const char *child)
{
    lock_acquire(dir->lock);
    bool has_entry = directory_entry(dir, child) != NULL;
    lock_release(dir->lock);

    return has_entry;
}

directory_entries_t directory_entries(fsnode_t *dir)
{
    lock_acquire(dir->lock);

    int entries_count = dir->directory.childs->count;
    iostream_direntry_t *entries = malloc(sizeof(iostream_direntry_t) * entries_count);
    iostream_direntry_t *current = &entries[0];

    list_foreach(i, dir->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;

        strlcpy(current->name, entry->name, PATH_ELEMENT_LENGHT);
        fsnode_stat(entry->node, &current->stat);

        current++;
    }

    lock_release(dir->lock);

    return (directory_entries_t){.count = entries_count, .entries = entries};
}

bool directory_link(fsnode_t *dir, fsnode_t *child, const char *name)
{
    lock_acquire(dir->lock);

    if (directory_entry(dir, name) == NULL)
    {
        fsdirectory_entry_t *entry = MALLOC(fsdirectory_entry_t);

        strlcpy(entry->name, name, PATH_ELEMENT_LENGHT);
        child->refcount++;
        entry->node = child;

        list_pushback(dir->directory.childs, entry);

        lock_release(dir->lock);
        return true;
    }
    else
    {
        lock_release(dir->lock);
        return false;
    }
}

bool directory_unlink(fsnode_t *dir, const char *name)
{
    lock_acquire(dir->lock);
    fsdirectory_entry_t *entry = directory_entry(dir, name);

    if (entry != NULL)
    {
        fsnode_t *node = entry->node;

        list_remove(dir->directory.childs, entry);
        free(entry);

        node->refcount--;

        if (node->refcount == 0)
        {
            fsnode_delete(node);
        }

        lock_release(dir->lock);
        return true;
    }
    else
    {
        lock_release(dir->lock);
        return false;
    }
}

int directory_read(stream_t *stream, void *buffer, uint size)
{
    int index = stream->offset / sizeof(iostream_direntry_t);

    if (size == sizeof(iostream_direntry_t))
    {
        if (index < stream->direntries.count)
        {
            int entrysize = sizeof(iostream_direntry_t);

            memcpy(buffer, &stream->direntries.entries[index], entrysize);
            stream->offset += entrysize;

            return entrysize;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        logger_log(LOG_WARNING, "Directory read fail!");
        return -1;
    }
}

/* --- Filesystem ----------------------------------------------------------- */

void filesystem_setup()
{
    lock_init(fslock);

    root = fsnode(FSNODE_DIRECTORY);
    root->refcount++;

    filesystem_ready = true;
}

fsnode_t *filesystem_resolve(fsnode_t *at, path_t *p)
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

fsnode_t *filesystem_resolve_parent(fsnode_t *at, path_t *p)
{
    const char *child_name = path_pop(p);
    fsnode_t *child = filesystem_resolve(at, p);
    path_push(p, child_name);

    return child;
}

fsnode_t *filesystem_mknode(fsnode_t *at, path_t *node_path, fsnode_type_t type)
{
    IS_FS_READY;

    fsnode_t *parent_node = filesystem_resolve_parent(at, node_path);

    if (parent_node == NULL || parent_node->type != FSNODE_DIRECTORY)
    {
        logger_log(LOG_WARNING, "Failed to create new node, parent not found!");
        return NULL;
    }

    const char *child_name = path_filename(node_path);

    if (child_name == NULL || directory_has_entry(parent_node, child_name))
    {
        logger_log(LOG_WARNING, "Failed to create new node, the file exist!");
        return NULL;
    }

    fsnode_t *child_node = fsnode(type);

    if (!directory_link(parent_node, child_node, child_name))
    {
        logger_log(LOG_WARNING, "Failed to create new node, link() failed!");
        fsnode_delete(child_node);
        return NULL;
    }

    return child_node;
}

fsnode_t *filesystem_acquire(fsnode_t *at, path_t *p, bool create)
{
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
    if (filesystem_ready)
    {
        printf("\n\tFile system:\n");
        filesystem_dump_internal(root, 0);
    }
}

/* --- Filesystem Operations ------------------------------------------------ */

#define OPEN_OPTION(__opt) ((flags & __opt) && 1)
stream_t *filesystem_open(fsnode_t *at, path_t *p, iostream_flag_t flags)
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

        stream_delete(s);
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
    }
}

 stream_t* filesystem_dup(stream_t* s)
{
    s->node->refcount++;
    stream_t* dup = stream(s->node, s->flags);
    return dup;
}

int filesystem_read(stream_t *s, void *buffer, uint size)
{
    IS_FS_READY;

    if (size == 0) return 0;

    int result = -1;
    if (s != NULL)
    {
        if (s->flags & IOSTREAM_READ)
        {
            switch (s->node->type)
            {
            case FSNODE_FILE:
            {
                result = file_read(s, buffer, size);
                break;
            }

            case FSNODE_DEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->read != NULL)
                {
                    result = dev->read(s, buffer, size);
                }

                break;
            }

            case FSNODE_DIRECTORY:
            {
                result = directory_read(s, buffer, size);

                break;
            }

            case FSNODE_FIFO:
            {
                result = fifo_read(s, buffer, size);
                while (result == 0)
                {
                    task_sleep(10);
                    result = fifo_read(s, buffer, size);
                }

                break;
            }

            default:
                break;
            }
        }
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

int filesystem_write(stream_t *s, const void *buffer, uint size)
{
    IS_FS_READY;

    if (size == 0) return 0;

    int result = -1;

    if (s != NULL)
    {
        if (s->flags & IOSTREAM_WRITE)
        {
            switch (s->node->type)
            {
            case FSNODE_FILE:
            {
                result = file_write(s, buffer, size);

                break;
            }

            case FSNODE_DEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->write != NULL)
                {
                    result = dev->write(s, buffer, size);
                }

                break;
            }

            case FSNODE_FIFO:
            {
                result = fifo_write(s, buffer, size);
                while (result == 0)
                {
                    task_sleep(10);
                    result = fifo_write(s, buffer, size);
                }
                break;
            }

            default:
                break;
            }
        }
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

int filesystem_ioctl(stream_t *s, int request, void *args)
{
    IS_FS_READY;

    int result = -1;

    if (s != NULL)
    {
        if (s->node->type == FSNODE_DEVICE)
        {
            device_t *device = &s->node->device;

            if (device->ioctl != NULL)
            {
                result = device->ioctl(s, request, args);
            }
        }
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

int filesystem_fstat(stream_t *s, iostream_stat_t *stat)
{
    IS_FS_READY;

    if (s != NULL)
    {
        return fsnode_stat(s->node, stat);
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
        return 1;
    }
}

int filesystem_seek(stream_t *s, int offset, iostream_whence_t origine)
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
                s->offset = s->direntries.count * sizeof(iostream_direntry_t) + offset;
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

        return s->offset;
    }
    else
    {
        logger_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_tell(stream_t *s, iostream_whence_t whence)
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
        logger_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_mkdir(fsnode_t *at, path_t *p)
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

int filesystem_mkdev(fsnode_t *at, path_t *p, device_t dev)
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

int filesystem_mkfile(fsnode_t *at, path_t *p)
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

int filesystem_mkfifo(fsnode_t *at, path_t *p)
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

int filesystem_link(fsnode_t *file_at, path_t *file_path, fsnode_t *link_at, path_t *link_path)
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

int filesystem_unlink(fsnode_t *at, path_t *link_path)
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

int filesystem_rename(fsnode_t *atoldpath, path_t *oldpath, fsnode_t *atnewpath, path_t *newpath)
{
    IS_FS_READY;

    int result = -1;

    lock_acquire(fslock);
    {
        fsnode_t* node = filesystem_resolve(atoldpath, oldpath);
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

bool filesystem_exist(fsnode_t* at, path_t* p)
{
    fsnode_t* node = filesystem_acquire(at, p, false);
    
    if (node != NULL)
    {
        filesystem_release(node);
    }

    return node != NULL;
}
