
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

static void directory_entry_destroy(FsDirectoryEntry *entry)
{
    entry->node->deref();
    free(entry);
}

FsDirectory::FsDirectory() : FsNode(FILE_TYPE_DIRECTORY)
{
    _childs = list_create();
}

FsDirectory::~FsDirectory()
{
    list_destroy_with_callback(_childs, (ListDestroyElementCallback)directory_entry_destroy);
}

Result FsDirectory::open(FsHandle *handle)
{
    DirectoryListing *listing = (DirectoryListing *)malloc(sizeof(DirectoryListing) + sizeof(DirectoryEntry) * _childs->count());

    listing->count = _childs->count();

    int current_index = 0;

    list_foreach(FsDirectoryEntry, entry, _childs)
    {
        DirectoryEntry *record = &listing->entries[current_index];
        FsNode *node = entry->node;

        strcpy(record->name, entry->name);

        record->stat.type = node->type;
        record->stat.size = node->size();

        current_index++;
    };

    handle->attached = listing;

    return SUCCESS;
}

void FsDirectory::close(FsHandle *handle)
{
    free(handle->attached);
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

FsNode *FsDirectory::find(const char *name)
{
    list_foreach(FsDirectoryEntry, entry, _childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            entry->node->ref();
            return entry->node;
        }
    };

    return nullptr;
}

Result FsDirectory::link(const char *name, FsNode *child)
{
    list_foreach(FsDirectoryEntry, entry, _childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            return ERR_FILE_EXISTS;
        }
    };

    FsDirectoryEntry *new_entry = __create(FsDirectoryEntry);

    child->ref();
    new_entry->node = child;
    strcpy(new_entry->name, name);

    list_pushback(_childs, new_entry);

    return SUCCESS;
}

Result FsDirectory::unlink(const char *name)
{
    list_foreach(FsDirectoryEntry, entry, _childs)
    {
        if (strcmp(entry->name, name) == 0)
        {
            list_remove(_childs, entry);
            directory_entry_destroy(entry);

            return SUCCESS;
        }
    }

    return ERR_NO_SUCH_FILE_OR_DIRECTORY;
}
