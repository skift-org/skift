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

fsnode_t *directory_getchild(fsnode_t *dir, const char *child)
{
    FOREACH(i, dir->directory.childs)
    {
        fsnode_t *d = (fsnode_t *)i->value;

        if (strcmp(child, d->name) == 0)
        {
            return d;
        }
    }

    return NULL;
}

fsnode_t *filesystem_resolve(const char *path)
{
    fsnode_t *current = root;

    char buffer[FSNAME_SIZE];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (current->type == FSDIRECTORY)
        {
            current = directory_getchild(current, buffer);

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

/* --- Public functions ----------------------------------------------------- */

void filesystem_setup()
{
    sk_lock_init(fslock);
    root = fsnode("ROOT", FSDIRECTORY);
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

#define RETURN_OPEN_FAIL(why)                         \
    {                                                 \
        sk_lock_release(fslock);                      \
        sk_log(LOG_DEBUG, "File open fail: %s", why); \
        return NULL;                                  \
    }

#define OPEN_OPTION(__opt) ((option & __opt) && 1)

fsnode_t *filesystem_open(const char *path, fsopenopt_t option)
{
    sk_lock_acquire(fslock);

    fsnode_t *node = filesystem_resolve(path);

    if (node == NULL && OPEN_OPTION(OPENOPT_CREATE))
    {
        char *parent_path = malloc(strlen(path));
        char *child_name = malloc(FSNAME_SIZE);

        if (path_split(path, parent_path, child_name))
        {
            fsnode_t *parent = filesystem_resolve(parent_path);

            if (parent->type == FSDIRECTORY)
            {
                node = fsnode(child_name, FSFILE);
                node->refcount++;
                list_pushback(parent->directory.childs, node);
            }
        }

        free(parent_path);
        free(child_name);
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
                RETURN_OPEN_FAIL("The file is already appened");
            }
            break;

        case FSDIRECTORY:
            RETURN_OPEN_FAIL("It's a directory");
            break;

        default:
            RETURN_OPEN_FAIL("\\(o_o)/");
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

#define READ_FAIL(reason, why)                         \
    {                                                  \
        sk_lock_release(node->lock);                   \
        sk_log(LOG_DEBUG, "File read fail!: %s", why); \
        return reason;                                 \
    }

int filesystem_read(fsnode_t *node, uint offset, uint size, void *buffer)
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
                    READ_FAIL(FSRESULT_EOF, "End of file!");

                memcpy(buffer, (byte *)file->buffer + offset, min(file->size - offset, size));
                result = min(file->size - offset, size);
            }
            else
            {
                READ_FAIL(FSRESULT_READNOTPERMITTED, "This file is not opened for write");
            }
        }
        break;

        case FSDEVICE:
            READ_FAIL(FSRESULT_NOTSUPPORTED, "Device file are WIP.");
            break;

        default:
            READ_FAIL(FSRESULT_NOTSUPPORTED, "NOT SUPPORTED FSOBJECT");
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

void *filesystem_readall(fsnode_t *node)
{
    file_stat_t stat = {0};
    filesystem_fstat(node, &stat);
    void *buffer = malloc(stat.size);
    filesystem_read(node, 0, stat.size, buffer);

    return buffer;
}

#define WRITE_FAIL(reason)                     \
    {                                          \
        sk_lock_release(node->lock);           \
        sk_log(LOG_DEBUG, "File write fail!"); \
        return reason;                         \
    }

int filesystem_write(fsnode_t *node, uint offset, uint size, void *buffer)
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
                if (file->realsize < (offset + size))
                {
                    node->file.buffer = realloc(node->file.buffer, offset + size);
                    node->file.realsize = offset + size;
                }

                node->file.size = max(offset + size, node->file.size);
                memcpy((byte *)(file->buffer) + offset, buffer, size);
                result = size;
            }
            else
            {
                WRITE_FAIL(FSRESULT_READNOTPERMITTED);
            }
        }
        break;

        case FSDEVICE:
            WRITE_FAIL(FSRESULT_NOTSUPPORTED);
            break;

        default:
            WRITE_FAIL(FSRESULT_NOTSUPPORTED);
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

#define STAT_FAIL(reason)                      \
    {                                          \
        sk_lock_release(node->lock);           \
        sk_log(LOG_DEBUG, "File state fail!"); \
        return reason;                         \
    }

int filesystem_fstat(fsnode_t *node, file_stat_t *stat)
{
    if (node != NULL)
    {
        sk_lock_acquire(node->lock);
        {
            stat->type = node->type;

            if (node->type == FSFILE)
            {
                stat->size = node->file.size;
            }
            else
            {
                stat->size = 0;
            }
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

int filesystem_mkdev(const char* path, device_t dev)
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

int filesystem_mkfile(const char* path)
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