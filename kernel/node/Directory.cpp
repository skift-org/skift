
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

static Result directory_open(FsDirectory *node, FsHandle *handle)
{
    DirectoryListing *listing = (DirectoryListing *)malloc(sizeof(DirectoryListing) + sizeof(DirectoryEntry) * node->childs->count());

    listing->count = node->childs->count();

    int current_index = 0;

    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        DirectoryEntry *record = &listing->entries[current_index];
        FsNode *node = entry->node;

        strcpy(record->name, entry->name);
        record->stat.type = node->type;

        if (node->size)
        {
            record->stat.size = node->size(entry->node, nullptr);
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

static FsNode *directory_find(FsDirectory *node, const char *name)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            return entry->node->ref();
        }
    };

    return nullptr;
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

    new_entry->node = child->ref();
    strcpy(new_entry->name, name);

    list_pushback(node->childs, new_entry);

    return SUCCESS;
}

static void directory_entry_destroy(FsDirectoryEntry *entry)
{
    entry->node->deref();
    free(entry);
}

static Result directory_unlink(FsDirectory *node, const char *name)
{
    list_foreach(FsDirectoryEntry, entry, node->childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            list_remove(node->childs, entry);
            directory_entry_destroy(entry);

            return SUCCESS;
        }
    }

    return ERR_NO_SUCH_FILE_OR_DIRECTORY;
}

static void directory_destroy(FsDirectory *node)
{
    list_destroy_with_callback(node->childs, (ListDestroyElementCallback)directory_entry_destroy);
}

FsDirectory::FsDirectory() : FsNode(FILE_TYPE_DIRECTORY)
{
    open = (FsNodeOpenCallback)directory_open;
    close = (FsNodeCloseCallback)directory_close;
    find = (FsNodeFindCallback)directory_find;
    link = (FsNodeLinkCallback)directory_link;
    unlink = (FsNodeUnlinkCallback)directory_unlink;
    destroy = (FsNodeDestroyCallback)directory_destroy;

    childs = list_create();
}

ResultOr<size_t> FsDirectory::read(FsHandle &handle, void *buffer, size_t size)
{
    if (size == sizeof(DirectoryEntry))
    {
        size_t index = handle.offset / sizeof(DirectoryEntry);

        DirectoryListing *listing = (DirectoryListing *)handle.attached;

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
