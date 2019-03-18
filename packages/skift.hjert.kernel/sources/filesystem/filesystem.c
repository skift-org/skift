/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

/*
 * TODO:
 *  - A lot of error checking is missing
 *  - Add support for on disk file system 
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <skift/logger.h>

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

#pragma region

fsnode_t *fsnode(fsnode_type_t type)
{
    fsnode_t *node = MALLOC(fsnode_t);

    node->type = type;
    node->refcount = 0;

    switch (type)
    {
    case FSFILE:
    {
        file_t *file = &node->file;

        file->buffer = malloc(512);
        file->realsize = 512;
        file->size = 0;
        break;
    }
    case FSDIRECTORY:
    {
        directory_t *dir = &node->directory;

        dir->childs = list();
        break;
    }
    case FSFIFO:
    {
        fifo_t *fifo = &node->fifo;
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
    sk_log(LOG_DEBUG, "Fsnode free: '%s'", node->name);

    switch (node->type)
    {
    case FSFILE:
    {
        free(node->file.buffer);
        break;
    }
    case FSDIRECTORY:
    {
        FOREACH(item, node->directory.childs)
        {
            fsdirectory_entry_t *entry = item->value;
            fsnode_t *n = entry->node;

            free(entry);

            n->refcount--;

            if (n->refcount == 0)
            {
                fsnode_delete(n);
            }
        }

        list_delete(node->directory.childs);
        break;
    }
    case FSFIFO:
    {
        ringbuffer_delete(node->fifo.buffer);
        break;
    }
    default:
        break;
    }

    free(node);
}

#pragma endregion

/* --- Streams -------------------------------------------------------------- */

#pragma region

stream_t *stream(fsnode_t *node, fsoflags_t flags)
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

#pragma endregion

/* --- Files ---------------------------------------------------------------- */

#pragma region

void file_trunc(fsnode_t *node)
{
    sk_lock_acquire(node->lock);

    free(node->file.buffer);

    node->file.buffer = malloc(512);
    node->file.realsize = 512;
    node->file.size = 0;

    sk_lock_release(node->lock);
}

int file_read(stream_t *stream, void *buffer, uint size)
{
    int result = 0;

    sk_lock_acquire(stream->node->lock);

    file_t *file = &stream->node->file;

    if (stream->offset <= file->size)
    {
        int readedsize = min(file->size - stream->offset, size);
        memcpy(buffer, (byte *)file->buffer + stream->offset, readedsize);

        result = readedsize;
        stream->offset += readedsize;
    }

    sk_lock_release(stream->node->lock);

    return result;
}

int file_write(stream_t *stream, void *buffer, uint size)
{
    int result = 0;

    sk_lock_acquire(stream->node->lock);

    file_t *file = &stream->node->file;

    if (stream->flags & OPENOPT_APPEND)
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

    sk_lock_release(stream->node->lock);

    return result;
}

void file_stat(fsnode_t *node, file_stat_t *stat)
{
    sk_lock_acquire(node->lock);

    stat->size = node->file.size;

    sk_lock_release(node->lock);
}

#pragma endregion

/* --- Directories ---------------------------------------------------------- */

#pragma region

// only call this method if you old the directory lock.
fsdirectory_entry_t *directory_entry(fsnode_t *dir, const char *child)
{
    FOREACH(i, dir->directory.childs)
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
    sk_lock_acquire(dir->lock);
    bool has_entry = directory_entry(dir, child) != NULL;
    sk_lock_release(dir->lock);

    return has_entry;
}

directory_entries_t directory_entries(fsnode_t *dir)
{
    sk_lock_acquire(dir->lock);

    int entries_count = dir->directory.childs->count;
    directory_entry_t *entries = malloc(sizeof(directory_entry_t) * entries_count);
    directory_entry_t *current = &entries[0];

    FOREACH(i, dir->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;

        strncpy(current->name, entry->name, MAX_FILENAME_LENGHT);
        current->type = entry->node->type;

        current++;
    }

    sk_lock_release(dir->lock);

    return (directory_entries_t){.count = entries_count, .entries = entries};
}

bool directory_link(fsnode_t *dir, fsnode_t *child, const char *name)
{
    sk_lock_acquire(dir->lock);
    sk_log(LOG_DEBUG, "Linking node to '%s'", name);

    if (directory_entry(dir, name) == NULL)
    {
        fsdirectory_entry_t *entry = MALLOC(fsdirectory_entry_t);

        strncpy(entry->name, name, MAX_FILENAME_LENGHT);
        child->refcount++;
        entry->node = child;

        list_pushback(dir->directory.childs, entry);

        sk_lock_release(dir->lock);
        return true;
    }
    else
    {
        sk_lock_release(dir->lock);
        return false;
    }
}

bool directory_unlink(fsnode_t *dir, const char *name)
{
    sk_lock_acquire(dir->lock);
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

        sk_lock_release(dir->lock);
        return true;
    }
    else
    {
        sk_lock_release(dir->lock);
        return false;
    }
}

int directory_read(stream_t *stream, void *buffer, uint size)
{
    int index = stream->offset / sizeof(directory_entry_t);

    if (size == sizeof(directory_entry_t))
    {
        sk_log(LOG_DEBUG, "Entry count %d", stream->direntries.count);
        if (index < stream->direntries.count)
        {
            int entrysize = sizeof(directory_entry_t);

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
        sk_log(LOG_DEBUG, "Directory read fail!");
        return -1;
    }
}

#pragma endregion

/* --- Filesystem ----------------------------------------------------------- */

#pragma region

void filesystem_setup()
{
    sk_lock_init(fslock);

    root = fsnode(FSDIRECTORY);
    root->refcount++;

    filesystem_ready = true;

    filesystem_mkdir(NULL, "/dev");
    filesystem_mkdir(NULL, "/sys");
}

fsnode_t *filesystem_resolve(fsnode_t *at, const char *path)
{
    fsnode_t *current = (at == NULL) ? root : at;

    char buffer[MAX_FILENAME_LENGHT];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (current->type == FSDIRECTORY)
        {
            fsnode_t* directory = current;
            sk_lock_acquire(directory->lock);
            fsdirectory_entry_t *entry = directory_entry(current, buffer);

            if (entry != NULL)
            {
                current = entry->node;
                sk_lock_release(directory->lock);
            }
            else
            {
                sk_lock_release(directory->lock);
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

fsnode_t *filesystem_acquire(fsnode_t *at, const char *path, bool create)
{
    sk_lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(at, path);

    if (node == NULL && create)
    {
        char *parent_path = malloc(strlen(path));
        char *child_name = malloc(MAX_FILENAME_LENGHT);

        if (path_split(path, parent_path, child_name))
        {
            fsnode_t *parent = filesystem_resolve(at, parent_path);

            if (parent != NULL && parent->type == FSDIRECTORY)
            {
                node = fsnode(FSFILE);
                directory_link(parent, node, child_name);
            }
        }

        free(parent_path);
        free(child_name);
    }

    if (node != NULL)
    {
        node->refcount++;
    }

    sk_lock_release(fslock);

    return node;
}

fsnode_t *filesystem_mknode(fsnode_t *at, const char *path, fsnode_type_t type)
{
    IS_FS_READY;

    fsnode_t *child = NULL;

    sk_lock_acquire(fslock);
    {
        sk_log(LOG_DEBUG, "Creating node '%s' of type %d...", path, type);
        
        char *parent_name = malloc(strlen(path));
        char *child_name = malloc(MAX_FILENAME_LENGHT);

        if (path_split(path, parent_name, child_name))
        {
            fsnode_t *parent = filesystem_resolve(at, parent_name);

            if (parent->type == FSDIRECTORY && !directory_has_entry(parent, child_name))
            {
                child = fsnode(type);
                directory_link(parent, child, child_name);
            }
        }

        free(child_name);
        free(parent_name);
    }
    sk_lock_release(fslock);

    return child;
}

void filesystem_release(fsnode_t *node)
{
    sk_lock_acquire(fslock);

    node->refcount--;

    if (node->refcount == 0)
    {
        fsnode_delete(node);
    }

    sk_lock_release(fslock);
}

void filesystem_dump_internal(fsnode_t *node, int depth)
{
    FOREACH(i, node->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;
        fsnode_t *node = entry->node;

        for (int i = 0; i < depth; i++)
        {
            printf("  ");
        }

        if (node->type == FSFILE)
        {
            printf("  '%s' size: %dbytes\n", entry->name, node->file.size);
        }
        else if (node->type == FSDIRECTORY)
        {
            printf("* '%s' %d childs\n", entry->name, node->directory.childs->count);
            filesystem_dump_internal(node, depth + 1);
        }
        else
        {
            printf("! '%s'\n", node->name);
        }
    }
}

void filesystem_dump(void)
{
    sk_lock_acquire(fslock);

    printf("\n--- FILE SYSTEM DUMP -----------------------------------------------------------\n\n");

    filesystem_dump_internal(root, 0);

    printf("\n--------------------------------------------------------------------------------\n\n");

    sk_lock_release(fslock);
}

#pragma endregion

/* --- Filesystem Operations ------------------------------------------------ */

#pragma region

#define OPEN_OPTION(__opt) ((flags & __opt) && 1)
stream_t *filesystem_open(fsnode_t *at, const char *path, fsoflags_t flags)
{
    IS_FS_READY;

    fsnode_t *node = filesystem_acquire(at, path, OPEN_OPTION(OPENOPT_CREATE));

    if (node == NULL)
    {
        return NULL;
    }
    else
    {
        stream_t *s = stream(node, flags);

        if (node->type == FSFILE)
        {
            if (OPEN_OPTION(OPENOPT_TRUNC))
                file_trunc(node);
        }
        else if (node->type == FSDIRECTORY)
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

        if (type == FSDIRECTORY)
        {
            free(s->direntries.entries);
        }

        stream_delete(s);
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
    }
}

int filesystem_read(stream_t *s, void *buffer, uint size)
{
    IS_FS_READY;

    int result = -1;
    if (s != NULL)
    {
        if (s->flags & OPENOPT_READ || s->flags & OPENOPT_READWRITE)
        {
            switch (s->node->type)
            {
            case FSFILE:
            {
                result = file_read(s, buffer, size);
                break;
            }

            case FSDEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->read != NULL)
                {
                    result = dev->read(s, buffer, size);
                }

                break;
            }

            case FSDIRECTORY:
            {
                result = directory_read(s, buffer, size);

                break;
            }

            case FSFIFO:
            {
                result = ringbuffer_read(s->node->fifo.buffer, buffer, size);

                break;
            }

            default:
                break;
            }
        }
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

void *filesystem_readall(stream_t *s)
{
    IS_FS_READY;

    if (s != NULL)
    {
        file_stat_t stat = {0};
        filesystem_fstat(s, &stat);
        void *buffer = malloc(stat.size);
        filesystem_read(s, buffer, stat.size);

        return buffer;
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
        return NULL;
    }
}

int filesystem_write(stream_t *s, void *buffer, uint size)
{
    IS_FS_READY;

    int result = -1;

    if (s != NULL)
    {
        if ((s->flags & OPENOPT_WRITE) || (s->flags & OPENOPT_READWRITE))
        {
            switch (s->node->type)
            {
            case FSFILE:
            {
                result = file_write(s, buffer, size);

                break;
            }

            case FSDEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->read != NULL)
                {
                    result = dev->write(s, buffer, size);
                }

                break;
            }

            case FSFIFO:
            {
                result = ringbuffer_write(s->node->fifo.buffer, buffer, size);

                break;
            }

            default:
                break;
            }
        }
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

int filesystem_ioctl(stream_t *s, int request, void *args)
{
    IS_FS_READY;

    int result = -1;

    if (s != NULL)
    {
        if (s->node->type == FSDEVICE)
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
        sk_log(LOG_WARNING, "Null stream passed");
    }

    return result;
}

int filesystem_fstat(stream_t *s, file_stat_t *stat)
{
    IS_FS_READY;

    if (s != NULL)
    {
        stat->type = s->node->type;
        stat->size = 0;

        if (s->node->type == FSFILE)
        {
            file_stat(s->node, stat);
        }

        return 0;
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
        return 1;
    }
}

int filesystem_seek(stream_t *s, int offset, seek_origin_t origine)
{
    IS_FS_READY;

    if (s != NULL)
    {
        switch (origine)
        {
        case SEEKFROM_START:
            s->offset = offset;
            break;

        case SEEKFROM_HERE:
            s->offset += offset;
            break;

        case SEEKFROM_END:
            if (s->node->type == FSFILE)
            {
                sk_lock_acquire(s->node->lock);
                s->offset = s->node->file.size + offset;
                sk_lock_release(s->node->lock);
            }
            else if (s->node->type == FSDIRECTORY)
            {
                s->offset = s->direntries.count * sizeof(directory_entry_t) + offset;
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
        sk_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_tell(stream_t *s)
{
    IS_FS_READY;

    if (s != NULL)
    {
        return s->offset;
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_mkdir(fsnode_t *at, const char *path)
{
    IS_FS_READY;

    if (filesystem_mknode(at, path, FSDIRECTORY) != NULL)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int filesystem_mkdev(fsnode_t *at, const char *path, device_t dev)
{
    IS_FS_READY;

    fsnode_t *device_file = filesystem_mknode(at, path, FSDEVICE);

    if (device_file != NULL)
    {
        device_file->device = dev;
        return 0;
    }
    else
    {
        return -1;
    }
}

int filesystem_mkfile(fsnode_t *at, const char *path)
{
    IS_FS_READY;

    if (filesystem_mknode(at, path, FSFILE) != NULL)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int filesystem_link(fsnode_t *oldat, const char *oldpath, fsnode_t *newat, const char *newpath)
{
    IS_FS_READY;

    int result = -1;

    sk_lock_acquire(fslock);
    {
        fsnode_t* node = filesystem_resolve(oldat, oldpath);

        if (node != NULL)
        {
            char *parent_name = malloc(strlen(newpath));
            char *child_name = malloc(MAX_FILENAME_LENGHT);

            if (path_split(newpath, parent_name, child_name))
            {
                fsnode_t *parent = filesystem_resolve(newat, parent_name);

                if (parent != NULL && parent->type == FSDIRECTORY)
                {
                    if (directory_link(parent, node, child_name))
                    {
                        result = 0;
                    }
                }
            }

            free(child_name);
            free(parent_name);
        }
    }
    sk_lock_release(fslock);

    return result;
}

int filesystem_unlink(fsnode_t *at, const char *path)
{
    IS_FS_READY;

    int result = -1;

    sk_lock_acquire(fslock);
    {
        char *parent_name = malloc(strlen(path));
        char *child_name = malloc(MAX_FILENAME_LENGHT);

        if (path_split(path, parent_name, child_name))
        {
            fsnode_t *parent = filesystem_resolve(at, parent_name);

            if (parent != NULL && parent->type == FSDIRECTORY)
            {
                if (directory_unlink(parent, child_name))
                {
                    result = 0;
                }
            }
        }

        free(child_name);
        free(parent_name);
    }
    sk_lock_release(fslock);

    return result;
}

#pragma endregion