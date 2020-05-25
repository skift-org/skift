
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

static Result directory_open(FsDirectory *node, FsHandle *handle)
{
    DirectoryListing *listing = (DirectoryListing *)malloc(sizeof(DirectoryListing) + sizeof(DirectoryEntry) * list_count(node->childs));

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

static void directory_close(FsDirectory *node, FsHandle *handle)
{
    __unused(node);

    free(handle->attached);
}

static Result directory_read(FsDirectory *node, FsHandle *handle, void *buffer, uint size, size_t *read)
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

            *read = sizeof(DirectoryEntry);
        }
    }

    return SUCCESS;
}

static FsNode *directory_find(FsDirectory *node, const char *name)
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

static Result directory_link(FsDirectory *node, const char *name, FsNode *child)
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

static Result directory_unlink(FsDirectory *node, const char *name)
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

static void directory_destroy(FsDirectory *node)
{
    list_destroy_with_callback(node->childs, (ListDestroyElementCallback)fsnode_deref);
}

FsNode *directory_create(void)
{
    FsDirectory *directory = __create(FsDirectory);

    fsnode_init(FSNODE(directory), FILE_TYPE_DIRECTORY);

    FSNODE(directory)->open = (FsNodeOpenCallback)directory_open;
    FSNODE(directory)->close = (FsNodeCloseCallback)directory_close;
    FSNODE(directory)->read = (FsNodeReadCallback)directory_read;
    FSNODE(directory)->find = (FsNodeFindCallback)directory_find;
    FSNODE(directory)->link = (FsNodeLinkCallback)directory_link;
    FSNODE(directory)->unlink = (FsNodeUnlinkCallback)directory_unlink;
    FSNODE(directory)->destroy = (FsNodeDestroyCallback)directory_destroy;

    directory->childs = list_create();

    return (FsNode *)directory;
}
