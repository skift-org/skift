/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

Result directory_FsOperationOpen(FsDirectory *node, FsHandle *handle)
{
    DirectoryListing *listing = malloc(sizeof(DirectoryListing) + sizeof(DirectoryEntry) * list_count(node->childs));

    listing->count = list_count(node->childs);

    int current_index = 0;

    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        DirectoryEntry *record = &listing->entries[current_index];
        FsNode *node = entry->node;

        strcpy(record->name, entry->name);
        record->stat.type = node->type;

        if (node->size)
        {
            record->stat.size = node->size(entry->node, NULL);
        }
        else
        {
            record->stat.size = 0;
        }

        current_index++;
    };

    handle->attached = listing;

    return SUCCESS;
}

void directory_FsOperationClose(FsDirectory *node, FsHandle *handle)
{
    __unused(node);

    free(handle->attached);
}

Result directory_FsOperationRead(FsDirectory *node, FsHandle *handle, void *buffer, uint size, size_t *readed)
{
    __unused(node);
    // FIXME: directories should no be read using read().

    if (size == sizeof(DirectoryEntry))
    {
        size_t index = handle->offset / sizeof(DirectoryEntry);

        DirectoryListing *listing = (DirectoryListing *)handle->attached;

        if (index < listing->count)
        {
            *((DirectoryEntry *)buffer) = listing->entries[index];

            *readed = sizeof(DirectoryEntry);
        }
    }

    return SUCCESS;
}

FsNode *directory_FsOperationFind(FsDirectory *node, const char *name)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            return fsnode_ref(entry->node);
        }
    };

    return NULL;
}

Result directory_FsOperationLink(FsDirectory *node, const char *name, FsNode *child)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            return ERR_FILE_EXISTS;
        }
    };

    FsDirectoryEntry *new_entry = __create(FsDirectoryEntry);

    new_entry->node = fsnode_ref(child);
    strcpy(new_entry->name, name);

    list_pushback(node->childs, new_entry);

    return SUCCESS;
}

Result directory_FsOperationUnlink(FsDirectory *node, const char *name)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            list_remove(node->childs, entry);
            fsnode_deref(entry->node);
            free(entry);

            return SUCCESS;
        }
    }

    return ERR_NO_SUCH_FILE_OR_DIRECTORY;
}

void directory_FsOperationDestroy(FsDirectory *node)
{
    list_destroy_with_callback(node->childs, (ListDestroyElementCallback)fsnode_deref);
}

FsNode *directory_create(void)
{
    FsDirectory *directory = __create(FsDirectory);

    fsnode_init(FSNODE(directory), FSNODE_DIRECTORY);

    FSNODE(directory)->open = (FsOperationOpen)directory_FsOperationOpen;
    FSNODE(directory)->close = (FsOperationClose)directory_FsOperationClose;
    FSNODE(directory)->read = (FsOperationRead)directory_FsOperationRead;
    FSNODE(directory)->find = (FsOperationFind)directory_FsOperationFind;
    FSNODE(directory)->link = (FsOperationLink)directory_FsOperationLink;
    FSNODE(directory)->unlink = (FsOperationUnlink)directory_FsOperationUnlink;
    FSNODE(directory)->destroy = (FsOperationDestroy)directory_FsOperationDestroy;

    directory->childs = list_create();

    return (FsNode *)directory;
}