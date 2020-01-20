/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>

#include "node/Directory.h"
#include "node/Handle.h"

int directory_FsOperationOpen(FsDirectory *node, Handle *handle)
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

    return -ERR_SUCCESS;
}

void directory_FsOperationClose(FsDirectory *node, Handle *handle)
{
    __unused(node);

    free(handle->attached);
}

int directory_FsOperationRead(FsDirectory *node, Handle *handle, void *buffer, uint size)
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

            return sizeof(DirectoryEntry);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
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

int directory_FsOperationLink(FsDirectory *node, const char *name, FsNode *child)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            return -ERR_FILE_EXISTS;
        }
    };

    FsDirectoryEntry *new_entry = __create(FsDirectoryEntry);

    new_entry->node = fsnode_ref(child);
    strcpy(new_entry->name, name);

    list_pushback(node->childs, new_entry);

    return -ERR_SUCCESS;
}

int directory_FsOperationUnlink(FsDirectory *node, const char *name)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            fsnode_deref(entry->node);
            free(entry);

            return -ERR_SUCCESS;
        }
    };

    return -ERR_NO_SUCH_FILE_OR_DIRECTORY;
}

void directory_FsOperationDestroy(FsDirectory *node)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        fsnode_deref(entry->node);
    }

    list_destroy(node->childs, LIST_FREE_VALUES);
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