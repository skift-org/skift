/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <skift/logger.h>
#include <skift/ringbuffer.h>

#include "kernel/filesystem.h"

static fsnode_t *root = NULL;
static lock_t fslock;

/* --- Fsnode --------------------------------------------------------------- */

#pragma region

fsnode_t *fsnode(const char *name, fsnode_type_t type)
{
    fsnode_t *node = MALLOC(fsnode_t);

    node->type = type;
    strncpy(node->name, name, FSNAME_SIZE);
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
    default:
        break;
    }

    return node;
}

void fsnode_delete(fsnode_t *node)
{
    switch (node->type)
    {
    case FSFILE:
        free(node->file.buffer);
        break;

    case FSDIRECTORY:
        FOREACH(item, node->directory.childs)
        {
            fsnode_t *n = item->value;

            n->refcount--;

            if (n->refcount == 0)
            {
                fsnode_delete(n);
            }
        }

        list_delete(node->directory.childs);
        break;

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

    if (stream->offset <= size)
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

    if (file->realsize < (stream->offset + size))
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

fsnode_t *directory_child(fsnode_t *dir, const char *child)
{
    sk_lock_acquire(dir->lock);

    FOREACH(i, dir->directory.childs)
    {
        fsnode_t *d = (fsnode_t *)i->value;

        if (strcmp(child, d->name) == 0)
        {
            sk_lock_release(dir->lock);
            return d;
        }
    }

    sk_lock_release(dir->lock);

    return NULL;
}

directory_entries_t directory_entries(fsnode_t *dir)
{
    sk_lock_acquire(dir->lock);

    int entries_count = dir->directory.childs->count;
    directory_entry_t *entries = malloc(sizeof(directory_entry_t) * entries_count);
    directory_entry_t *current = &entries[0];

    FOREACH(i, dir->directory.childs)
    {
        fsnode_t *c = (fsnode_t *)i->value;

        strncpy(current->name, c->name, FSNAME_SIZE);
        current->type = c->type;

        current++;
    }

    sk_lock_release(dir->lock);

    return (directory_entries_t){.count = entries_count, .entries = entries};
}

void directory_addchild(fsnode_t *dir, fsnode_t *child)
{
    sk_lock_acquire(dir->lock);

    if (!list_containe(dir->directory.childs, child))
    {
        child->refcount++;
        list_pushback(dir->directory.childs, child);
    }

    sk_lock_release(dir->lock);
}

void directory_removechild(fsnode_t *dir, fsnode_t *child)
{
    sk_lock_acquire(dir->lock);

    if (list_containe(dir->directory.childs, child))
    {
        child->refcount--;
        if (child->refcount == 0)
        {
            fsnode_delete(child);
        }

        list_remove(dir->directory.childs, child);
    }

    sk_lock_release(dir->lock);
}

int directory_read(stream_t *stream, void *buffer, uint size)
{
    int index = stream->offset / sizeof(stream_t);

    if (size == sizeof(directory_entries_t))
    {
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
        return -1;
    }

}

#pragma endregion

/* --- Filesystem ----------------------------------------------------------- */

#pragma region

void filesystem_setup()
{
    sk_lock_init(fslock);
    root = fsnode("ROOT", FSDIRECTORY);
}

fsnode_t *filesystem_resolve(const char *path)
{
    fsnode_t *current = root;

    char buffer[FSNAME_SIZE];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (current->type == FSDIRECTORY)
        {
            current = directory_child(current, buffer);

            if (current == NULL)
            {
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

fsnode_t *filesystem_acquire(const char *path, bool create)
{
    sk_lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(path);

    if (node == NULL && create)
    {
        char *parent_path = malloc(strlen(path));
        char *child_name = malloc(FSNAME_SIZE);

        if (path_split(path, parent_path, child_name))
        {
            fsnode_t *parent = filesystem_resolve(parent_path);

            if (parent != NULL && parent->type == FSDIRECTORY)
            {
                node = fsnode(child_name, FSFILE);
                node->refcount++;
                list_pushback(parent->directory.childs, node);
            }
        }

        free(parent_path);
        free(child_name);
    }

    node->refcount++;

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
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    if (node->type == FSDIRECTORY)
    {
        printf("* '%s' %d childs\n", node->name, node->directory.childs->count);

        FOREACH(i, node->directory.childs)
        {
            fsnode_t *d = (fsnode_t *)i->value;

            filesystem_dump_internal(d, depth + 1);
        }
    }
    else if (node->type == FSFILE)
    {
        printf("  '%s' size: %dkio\n", node->name, node->file.size / 1024);
    }
    else
    {
        printf("! '%s'\n", node->name);
    }
}

void filesystem_dump(void)
{
    sk_lock_acquire(fslock);

    printf("--- FILE SYSTEM DUMP -----------------------------------------------------------\n");

    filesystem_dump_internal(root, 0);

    printf("--------------------------------------------------------------------------------\n");

    sk_lock_release(fslock);
}

#pragma endregion

/* --- Filesystem Operations ------------------------------------------------ */

#pragma region

#define OPEN_OPTION(__opt) ((flags & __opt) && 1)
stream_t *filesystem_open(const char *path, fsoflags_t flags)
{
    fsnode_t *node = filesystem_acquire(path, OPEN_OPTION(OPENOPT_CREATE));

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
    fsnode_type_t type = s->node->type;

    filesystem_release(s->node);

    if (type == FSDIRECTORY)
    {
        free(s->direntries.entries);
    }

    stream_delete(s);
}

int filesystem_read(stream_t *s, void *buffer, uint size)
{
    int result = -1;

    if (s->flags & OPENOPT_READ || s->flags & OPENOPT_READWRITE)
    {
        switch (s->node->type)
        {
        case FSFILE:
            result = file_read(s, buffer, size);
            break;

        case FSDEVICE:
        {
            device_t *dev = &s->node->device;

            if (dev->read != NULL)
            {
                result = dev->read(s, size, buffer);
            }
        }
        break;

        case FSDIRECTORY:
            result = directory_read(s, buffer, size);
            break;

        default:
            break;
        }
    }

    return result;
}

void *filesystem_readall(stream_t *s)
{
    file_stat_t stat = {0};
    filesystem_fstat(s, &stat);
    void *buffer = malloc(stat.size);
    filesystem_read(s, buffer, stat.size);

    return buffer;
}

int filesystem_write(stream_t *s, void *buffer, uint size)
{
    int result = -1;

    if (s->flags & OPENOPT_READ || s->flags & OPENOPT_READWRITE)
    {
        switch (s->node->type)
        {
        case FSFILE:
            result = file_write(s, buffer, size);
            break;

        case FSDEVICE:
        {
            device_t *dev = &s->node->device;

            if (dev->read != NULL)
            {
                result = dev->write(s, size, buffer);
            }
        }
        break;

        default:
            break;
        }
    }

    return result;
}

#define STAT_FAIL(reason)                      \
    {                                          \
        sk_lock_release(node->lock);           \
        sk_log(LOG_DEBUG, "File state fail!"); \
        return reason;                         \
    }

int filesystem_fstat(stream_t *s, file_stat_t *stat)
{
    stat->type = s->node->type;
    stat->size = 0;
    
    if (s->node->type == FSFILE)
    {
        file_stat(s->node, stat);
    }

    return 0;
}

int filesystem_mkdir(const char *path)
{
    int result = 0;

    sk_lock_acquire(fslock);
    {
        char *child = malloc(FSNAME_SIZE);
        char *parent = malloc(strlen(path));

        if (path_split(path, parent, child))
        {
            fsnode_t *p = filesystem_resolve(parent);

            if (p->type == FSDIRECTORY)
            {
                fsnode_t *c = fsnode(child, FSDIRECTORY);
                list_pushback(p->directory.childs, c);
                c->refcount++;

                result = FSRESULT_SUCCEED;
            }
        }

        free(child);
        free(parent);
    }
    sk_lock_release(fslock);

    return result;
}

int filesystem_mkdev(const char *path, device_t dev)
{
    int result = FSRESULT_FAILED;

    sk_lock_acquire(fslock);
    {
        char *child  = malloc(FSNAME_SIZE);
        char *parent = malloc(strlen(path));

        if (path_split(path, parent, child))
        {
            fsnode_t *p = filesystem_resolve(parent);

            if (p->type == FSDIRECTORY)
            {
                fsnode_t *c = fsnode(child, FSDEVICE);
                list_pushback(p->directory.childs, c);

                c->refcount++;
                c->device = dev;

                result = FSRESULT_SUCCEED;
            }
        }

        free(child);
        free(parent);
    }
    sk_lock_release(fslock);

    return result;
}

int filesystem_mkfile(const char *path)
{
    int result = FSRESULT_FAILED;

    sk_lock_acquire(fslock);
    {
        char *child = malloc(FSNAME_SIZE);
        char *parent = malloc(strlen(path));

        if (path_split(path, parent, child))
        {
            fsnode_t *p = filesystem_resolve(parent);

            if (p->type == FSDIRECTORY)
            {
                fsnode_t *c = fsnode(child, FSFILE);
                list_pushback(p->directory.childs, c);

                c->refcount++;

                result = FSRESULT_SUCCEED;
            }
        }

        free(child);
        free(parent);
    }
    sk_lock_release(fslock);

    return result;
}

#pragma endregion