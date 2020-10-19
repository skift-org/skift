
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

FsDirectory::FsDirectory() : FsNode(FILE_TYPE_DIRECTORY)
{
}

Result FsDirectory::open(FsHandle *handle)
{
    DirectoryListing *listing = (DirectoryListing *)malloc(sizeof(DirectoryListing) + sizeof(DirectoryEntry) * _childs.count());

    listing->count = _childs.count();

    int current_index = 0;

    _childs.foreach ([&](auto &entry) {
        auto record = &listing->entries[current_index];
        auto node = entry.node;

        strcpy(record->name, entry.name);

        record->stat.type = node->type();
        record->stat.size = node->size();

        current_index++;

        return Iteration::CONTINUE;
    });

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
        size_t index = handle.offset() / sizeof(DirectoryEntry);

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

RefPtr<FsNode> FsDirectory::find(const char *name)
{
    RefPtr<FsNode> result;

    _childs.foreach ([&](auto &entry) {
        if (strcmp(entry.name, name) == 0)
        {
            result = entry.node;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    return result;
}

Result FsDirectory::link(const char *name, RefPtr<FsNode> child)
{
    if (find(name))
    {
        return ERR_FILE_EXISTS;
    }

    FsDirectoryEntry entry = {};

    strcpy(entry.name, name);
    entry.node = child;

    _childs.push_back(entry);

    return SUCCESS;
}

Result FsDirectory::unlink(const char *name)
{
    bool has_removed_an_entry = _childs.remove_all_match(
        [&](auto &entry) {
            return strcmp(entry.name, name) == 0;
        });

    if (has_removed_an_entry)
    {
        return SUCCESS;
    }
    else
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }
}
