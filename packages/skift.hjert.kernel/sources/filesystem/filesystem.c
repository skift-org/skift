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

fsnode_t *fsnode(iostream_type_t type)
{
    fsnode_t *node = MALLOC(fsnode_t);

    node->type = type;
    node->refcount = 0;
    sk_lock_init(node->lock);

    switch (type)
    {
    case FILE_REGULAR:
    {
        file_t *file = &node->file;

        file->buffer = malloc(512);
        file->realsize = 512;
        file->size = 0;
        break;
    }
    case FILE_DIRECTORY:
    {
        directory_t *dir = &node->directory;

        dir->childs = list();
        break;
    }
    case FILE_FIFO:
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
    sk_log(LOG_DEBUG, "Fsnode free: %08X", node);

    switch (node->type)
    {
    case FILE_REGULAR:
    {
        free(node->file.buffer);
        break;
    }
    case FILE_DIRECTORY:
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

        list_delete(node->directory.childs, LIST_FREE_VALUES);
        break;
    }
    case FILE_FIFO:
    {
        ringbuffer_delete(node->fifo.buffer);
        break;
    }
    default:
        break;
    }

    free(node);
}

int fsnode_size(fsnode_t* node)
{
    if (node->type == FILE_REGULAR)
    {
        return node->file.size;
    }

    return -1;
}

#pragma endregion

/* --- Streams -------------------------------------------------------------- */

#pragma region

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

int file_write(stream_t *stream, const void *buffer, uint size)
{
    int result = 0;

    sk_lock_acquire(stream->node->lock);

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

    sk_lock_release(stream->node->lock);

    return result;
}

void file_stat(fsnode_t *node, iostream_stat_t *stat)
{
    sk_lock_acquire(node->lock);

    stat->size = node->file.size;

    sk_lock_release(node->lock);
}

#pragma endregion

/* --- Directories ---------------------------------------------------------- */

#pragma region

// only call this method if you hold the directory lock.
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
    iostream_direntry_t *entries = malloc(sizeof(iostream_direntry_t) * entries_count);
    iostream_direntry_t *current = &entries[0];

    FOREACH(i, dir->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;

        strncpy(current->name, entry->name, PATH_ELEMENT_LENGHT);
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

        strncpy(entry->name, name, PATH_ELEMENT_LENGHT);
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
    int index = stream->offset / sizeof(iostream_direntry_t);

    if (size == sizeof(iostream_direntry_t))
    {
        sk_log(LOG_DEBUG, "Entry count %d", stream->direntries.count);
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

    root = fsnode(FILE_DIRECTORY);
    root->refcount++;

    filesystem_ready = true;
}

fsnode_t *filesystem_resolve(fsnode_t *at, path_t* p)
{
    fsnode_t *current = (at == NULL) ? root : at;

    for(int i = 0; i < path_length(p); i++)
    {
        const char* element = path_element(p, i);

        if (current->type == FILE_DIRECTORY)
        {
            fsnode_t* directory = current;
            sk_lock_acquire(directory->lock);
            fsdirectory_entry_t *entry = directory_entry(current, element);

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

fsnode_t *filesystem_resolve_parent(fsnode_t *at, path_t* p)
{
    const char* child_name = path_pop(p);
    fsnode_t* child = filesystem_resolve(at, p);
    path_push(p, child_name);

    return child;
}

fsnode_t *filesystem_mknode(fsnode_t *at, path_t* node_path, iostream_type_t type)
{
    IS_FS_READY;

    sk_log(LOG_DEBUG, "Creating node '%s' of type %d...", path_filename(node_path), type);
    
    fsnode_t* parent_node = filesystem_resolve_parent(at, node_path);

    if (parent_node == NULL || parent_node->type != FILE_DIRECTORY)
    {
        return NULL;
    }

    const char* child_name = path_filename(node_path);

    if (child_name == NULL || directory_has_entry(parent_node, child_name))
    {
        return NULL;
    }

    fsnode_t* child_node = fsnode(type);

    if (!directory_link(parent_node, child_node, child_name))
    {
        fsnode_delete(child_node);
        return NULL;
    }

    return child_node;
}

fsnode_t *filesystem_acquire(fsnode_t *at, path_t* p, bool create)
{
    sk_lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(at, p);

    if (node == NULL && create)
    {
        node = filesystem_mknode(at, p, FILE_REGULAR);
    }

    if (node != NULL)
    {
        node->refcount++;
    }

    sk_lock_release(fslock);

    return node;
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


        if (node->type != FILE_DIRECTORY)
        {
            printf("\t");
            for (int i = 0; i < depth; i++)
            {
                printf("| \t");
            }

            if (node->type == FILE_REGULAR)
            {
                printf("|-%s size: %dbytes\n", entry->name, node->file.size);
            }
            else
            {
                printf("|-%s\n", entry->name);
            }
        }
    }

    FOREACH(i, node->directory.childs)
    {
        fsdirectory_entry_t *entry = (fsdirectory_entry_t *)i->value;
        fsnode_t *node = entry->node;

        if (node->type == FILE_DIRECTORY)
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

#pragma endregion

/* --- Filesystem Operations ------------------------------------------------ */

#pragma region

#define OPEN_OPTION(__opt) ((flags & __opt) && 1)
stream_t *filesystem_open(fsnode_t *at, path_t* p, iostream_flag_t flags)
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

        if (node->type == FILE_REGULAR)
        {
            if (OPEN_OPTION(IOSTREAM_TRUNC))
                file_trunc(node);
        }
        else if (node->type == FILE_DIRECTORY)
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
        iostream_type_t type = s->node->type;

        filesystem_release(s->node);

        if (type == FILE_DIRECTORY)
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
        if (s->flags & IOSTREAM_READ)
        {
            switch (s->node->type)
            {
            case FILE_REGULAR:
            {
                result = file_read(s, buffer, size);
                break;
            }

            case FILE_DEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->read != NULL)
                {
                    result = dev->read(s, buffer, size);
                }

                break;
            }

            case FILE_DIRECTORY:
            {
                result = directory_read(s, buffer, size);

                break;
            }

            case FILE_FIFO:
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

// TODO REMOVE: reading whole file is a bad idea
void *filesystem_readall(stream_t *s)
{
    IS_FS_READY;

    if (s != NULL)
    {
        iostream_stat_t stat = {0};
        filesystem_fstat(s, &stat);
        void *buffer = malloc(stat.size);
        int readed = filesystem_read(s, buffer, stat.size);


        int hash = 0;

        for(uint i = 0; i < stat.size; i++)
        {
            hash+= ((char*)buffer)[i];
        }
        
        sk_log(LOG_DEBUG, "readed %d out of %d hash:%08x", readed, stat.size, hash);

        return buffer;
    }
    else
    {
        sk_log(LOG_WARNING, "Null stream passed");
        return NULL;
    }
}

int filesystem_write(stream_t *s, const void *buffer, uint size)
{
    IS_FS_READY;

    int result = -1;

    if (s != NULL)
    {
        if (s->flags & IOSTREAM_WRITE)
        {
            switch (s->node->type)
            {
            case FILE_REGULAR:
            {
                result = file_write(s, buffer, size);

                break;
            }

            case FILE_DEVICE:
            {
                device_t *dev = &s->node->device;

                if (dev->read != NULL)
                {
                    result = dev->write(s, buffer, size);
                }

                break;
            }

            case FILE_FIFO:
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
        if (s->node->type == FILE_DEVICE)
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

int filesystem_fstat(stream_t *s, iostream_stat_t *stat)
{
    IS_FS_READY;

    if (s != NULL)
    {
        stat->type = s->node->type;
        stat->size = 0;

        if (s->node->type == FILE_REGULAR)
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
            if (s->node->type == FILE_REGULAR)
            {
                sk_lock_acquire(s->node->lock);
                s->offset = s->node->file.size + offset;
                sk_lock_release(s->node->lock);
            }
            else if (s->node->type == FILE_DIRECTORY)
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
        sk_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_tell(stream_t *s, iostream_whence_t whence)
{
    IS_FS_READY;

    if (s != NULL)
    {
        sk_lock_acquire(s->node->lock);
        int offset = s->offset;
        int size = fsnode_size(s->node);
        sk_lock_release(s->node->lock);

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
        sk_log(LOG_WARNING, "Null stream passed");
        return -1;
    }
}

int filesystem_mkdir(fsnode_t *at, path_t* p)
{
    IS_FS_READY;
    sk_lock_acquire(fslock);
    
    if (filesystem_mknode(at, p, FILE_DIRECTORY) != NULL)
    {
        sk_lock_release(fslock);
        return 0;
    }
    else
    {
        sk_lock_release(fslock);
        return -1;
    }
}

int filesystem_mkdev(fsnode_t *at, path_t* p, device_t dev)
{
    IS_FS_READY;
    sk_lock_acquire(fslock);

    fsnode_t *device_file = filesystem_mknode(at, p, FILE_DEVICE);

    if (device_file != NULL)
    {
        device_file->device = dev;
        sk_lock_release(fslock);
        return 0;
    }
    else
    {
        sk_lock_release(fslock);
        return -1;
    }
}

int filesystem_mkfile(fsnode_t *at, path_t* p)
{
    IS_FS_READY;
    sk_lock_acquire(fslock);
    if (filesystem_mknode(at, p, FILE_REGULAR) != NULL)
    {
        sk_lock_release(fslock);
        return 0;
    }
    else
    {
        sk_lock_release(fslock);
        return -1;
    }
}

int filesystem_link(fsnode_t * file_at, path_t* file_path, fsnode_t *link_at, path_t* link_path)
{
    IS_FS_READY;

    int result = -1;

    sk_lock_acquire(fslock);
    {
        fsnode_t* node = filesystem_resolve(file_at, file_path);

        if (node != NULL)
        {
            fsnode_t *parent = filesystem_resolve_parent(link_at, link_path);

            if (parent != NULL && parent->type == FILE_DIRECTORY)
            {
                if (directory_link(parent, node, path_filename(link_path)))
                {
                    result = 0;
                }
            }
        }
    }
    sk_lock_release(fslock);

    return result;
}

int filesystem_unlink(fsnode_t *at, path_t* link_path)
{
    IS_FS_READY;

    int result = -1;

    sk_lock_acquire(fslock);
    {
        fsnode_t *parent = filesystem_resolve_parent(at, link_path);

        if (parent != NULL && parent->type == FILE_DIRECTORY)
        {
            if (directory_unlink(parent, path_filename(link_path)))
            {
                result = 0;
            }
        }
    }
    sk_lock_release(fslock);

    return result;
}

#pragma endregion