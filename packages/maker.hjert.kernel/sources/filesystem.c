/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <skift/logger.h>

#include "kernel/filesystem.h"

static fsnode_t *root = NULL;
static lock_t fslock;

/* --- Constructor/Destructor ----------------------------------------------- */

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

        file->opened = false;

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
            // TODO: use refcount.
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

/* --- Private functions ---------------------------------------------------- */

void file_flush(fsnode_t *node)
{
    free(node->file.buffer);

    node->file.buffer = malloc(512);
    node->file.realsize = 512;
    node->file.size = 0;
}

fsnode_t *filesystem_resolve(const char *path)
{
    fsnode_t *current = root;

    char buffer[FSNAME_SIZE];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (current->type == FSDIRECTORY)
        {
            FOREACH(i, current->directory.childs)
            {
                fsnode_t *d = (fsnode_t *)i->value;

                if (strcmp(buffer, d->name) == 0)
                {
                    current = d;
                }
            }
        }
        else
        {
            return NULL;
        }
    }

    return current;
}

/* --- Public functions ----------------------------------------------------- */

void filesystem_setup()
{
    sk_lock_init(fslock);
    root = fsnode("ROOT", FSDIRECTORY);
}

#define RETURN_OPEN_FAIL()       \
    {                            \
        sk_lock_release(fslock); \
        return NULL;             \
    }

#define OPEN_OPTION(__opt) ((option & __opt) && 1)

fsnode_t *filesystem_open(const char *path, fsopenopt_t option)
{
    sk_lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(path);

    if (node == NULL && OPEN_OPTION(OPENOPT_CREATE))
    {
        char *child = malloc(FSNAME_SIZE);
        char *parent = malloc(strlen(path));

        if (path_split(path, parent, child))
        {
            fsnode_t *p = filesystem_resolve(parent);

            if (p->type == FSDIRECTORY)
            {
                node = fsnode(child, FSFILE);
                list_pushback(p->directory.childs, node);
            }
        }

        free(child);
        free(parent);
    }

    if (node != NULL)
    {
        switch (node->type)
        {
        case FSFILE:
            if (node->file.opened == false)
            {
                node->file.write = OPEN_OPTION(OPENOPT_WRITE);
                node->file.read = OPEN_OPTION(OPENOPT_READ);

                if (OPEN_OPTION(OPENOPT_WRITE))
                {
                    file_flush(node);
                }

                node->file.opened = true;
            }
            else
            {
                RETURN_OPEN_FAIL();
            }
            break;

        case FSDIRECTORY:
            RETURN_OPEN_FAIL();
            break;

        default:
            RETURN_OPEN_FAIL();
            break;
        }

        node->refcount++;
    }

    sk_lock_release(fslock);

    return node;
}

void filesystem_close(fsnode_t *node)
{
    sk_lock_acquire(fslock);

    if (node->type == FSFILE)
    {
        node->file.opened = false;
    }

    node->refcount--;
    if (node->refcount == 0)
    {
        fsnode_delete(node);
    }

    sk_lock_release(fslock);
}

#define READ_FAIL(reason)            \
    {                                \
        sk_lock_release(node->lock); \
        return reason;               \
    }

int filesystem_read(fsnode_t *node, uint offset, void *buffer, uint n)
{
    if (node != NULL)
    {
        sk_lock_acquire(node->lock);

        int result = 0;

        switch (node->type)
        {
        case FSFILE:
        {
            file_t *file = &node->file;

            if (file->read)
            {
                if (file->size < offset)
                    READ_FAIL(FSRESULT_EOF);

                memcpy(buffer, (byte *)file->buffer + offset, min(file->size - offset, n));
                result = min(file->size - offset, n);
            }
            else
            {
                READ_FAIL(FSRESULT_READNOTPERMITTED);
            }
        }
        break;

        case FSDEVICE:
            READ_FAIL(FSRESULT_NOTSUPPORTED);
            break;

        default:
            READ_FAIL(FSRESULT_NOTSUPPORTED);
            break;
        }

        sk_lock_release(node->lock);
        return result;
    }
    else
    {
        return FSRESULT_NULLNODE;
    }
}

void* filesystem_readall(fsnode_t *node)
{
    file_stat_t stat = {0};
    filesystem_stat(node, &stat);
    void* buffer = malloc(stat.size);
    filesystem_read(node, 0, buffer, stat.size);

    return buffer;
}

#define WRITE_FAIL(reason)           \
    {                                \
        sk_lock_release(node->lock); \
        return reason;               \
    }

int filesystem_write(fsnode_t *node, uint offset, void *buffer, uint n)
{
    if (node != NULL)
    {
        sk_lock_acquire(node->lock);

        int result = 0;

        switch (node->type)
        {
        case FSFILE:
        {
            file_t *file = &node->file;

            if (file->write)
            {
                if (file->realsize < (offset + n))
                {
                    node->file.buffer = realloc(node->file.buffer, offset + n);
                    node->file.realsize = offset + n;
                }

                node->file.size = max(offset + n, node->file.size);
                memcpy((byte *)(file->buffer) + offset, buffer, n);
                result = n;
            }
            else
            {
                READ_FAIL(FSRESULT_READNOTPERMITTED);
            }
        }
        break;

        case FSDEVICE:
            READ_FAIL(FSRESULT_NOTSUPPORTED);
            break;

        default:
            READ_FAIL(FSRESULT_NOTSUPPORTED);
            break;
        }

        sk_lock_release(node->lock);
        return result;
    }
    else
    {
        return FSRESULT_NULLNODE;
    }
}

#define STAT_FAIL(reason)            \
    {                                \
        sk_lock_release(node->lock); \
        return reason;               \
    }

int filesystem_stat(fsnode_t *node, file_stat_t *stat)
{
    if (node != NULL)
    {
        sk_lock_acquire(node->lock);

        stat->type = node->type;

        if (node->type == FSFILE)
        {
            stat->size = node->file.size;
        }
        else
        {
            stat->size = 0;
        }

        sk_lock_release(node->lock);

        return 0;
    }
    else
    {
        return FSRESULT_NULLNODE;
    }
}

int filesystem_mkdir(const char *path)
{
    int result = 0;

    sk_lock_acquire(fslock);
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

            result = 1;
        }
    }

    free(child);
    free(parent);
    sk_lock_release(fslock);

    return result;
}