#include <string.h>

#include <libsystem/Result.h>

#include "kernel/node/Directory.h"
#include "kernel/node/Handle.h"

FsDirectory::FsDirectory() : FsNode(FILE_TYPE_DIRECTORY)
{
}

Result FsDirectory::open(FsHandle &handle)
{
    FileListing *listing = (FileListing *)malloc(sizeof(FileListing) + sizeof(DirectoryEntry) * _childs.count());

    listing->count = _childs.count();

    int current_index = 0;

    _childs.foreach ([&](auto &entry) {
        auto record = &listing->entries[current_index];
        auto node = entry.node;

        strcpy(record->name, entry.name.cstring());

        record->stat.type = node->type();
        record->stat.size = node->size();

        current_index++;

        return Iteration::CONTINUE;
    });

    handle.attached = listing;

    return SUCCESS;
}

void FsDirectory::close(FsHandle &handle)
{
    free(handle.attached);
}

ResultOr<size_t> FsDirectory::read(FsHandle &handle, void *buffer, size_t size)
{
    if (size == sizeof(DirectoryEntry))
    {
        size_t index = handle.offset() / sizeof(DirectoryEntry);

        FileListing *listing = (FileListing *)handle.attached;

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

RefPtr<FsNode> FsDirectory::find(String name)
{
    RefPtr<FsNode> result;

    _childs.foreach ([&](auto &entry) {
        if (entry.name == name)
        {
            result = entry.node;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    return result;
}

Result FsDirectory::link(String name, RefPtr<FsNode> child)
{
    if (find(name))
    {
        return ERR_FILE_EXISTS;
    }

    _childs.push_back({name, child});

    return SUCCESS;
}

Result FsDirectory::unlink(String name)
{

    bool has_removed_an_entry = _childs.remove_all_match(
        [&](auto &e) {
            return e.name == name;
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
