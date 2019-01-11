/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* filesystem.c: the skiftOS virtual filesystem.                              */

#include <stdlib.h>
#include <string.h>

#include <skift/logger.h>
#include <skift/atomic.h>

#include "kernel/filesystem.h"

static directory_t *root = NULL;

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
        file_t *file = &node->file;

        file->opened = false;

        file->buffer = malloc(512);
        file->realsize = 512;
        file->size = 0;
        break;

    case FSDIRECTORY:
        directory_t *dir = &node->directory;

        dir->childs = list();
        break;

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
            fsnode_t* n = item->value;
            fsnode_delete(n);
        }

        list_delete(node->directory.childs); 
        break;

    default:
        break;
    }

    free(node);
}

/* --- Private functions ---------------------------------------------------- */

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
    root = fsnode("ROOT", FSDIRECTORY);
}

fsnode_t *filesystem_open(const char *path, fsopenopt_t option)
{
    sk_atomic_begin();

    fsnode_t *node = filesystem_resolve(path);

    if (node == NULL && option & OPENOPT_CREATE)
    {

    }

    sk_atomic_end();

    return node;
}

void filesystem_close(fsnode_t *node)
{
    sk_atomic_begin();

    if (node->type == FSFILE)
    {
        node->file.opened = false;
    }
    
    sk_atomic_end();
}

int  filesystem_read(fsnode_t *node, uint offset, void *buffer, uint n)
{

}

int filesystem_write(fsnode_t *node, uint offset, void *buffer, uint n)
{

}

int filesystem_mkdir(const char *path)
{
    int result = 0;

    char *child = malloc(FSNAME_SIZE);
    char *parent = malloc(strlen(path));

    if (path_split(path, parent, child))
    {
        fsnode_t* p = filesystem_resolve(parent);

        if (p->type == FSDIRECTORY)
        {
            fsnode_t* c = fsnode(child, FSDIRECTORY);
            list_pushback(p->directory.childs, c);

            result = 1;
        }
    }

    free(child);
    free(parent);

    return result;
}