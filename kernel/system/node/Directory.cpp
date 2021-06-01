#include <string.h>

#include <abi/Result.h>

#include "system/node/Directory.h"
#include "system/node/Handle.h"

FsDirectory::FsDirectory() : FsNode(HJ_FILE_TYPE_DIRECTORY)
{
}

HjResult FsDirectory::open(FsHandle &handle)
{
    FileListing *listing = (FileListing *)malloc(sizeof(FileListing) + sizeof(HjDirEntry) * _children.count());

    listing->count = _children.count();

    int current_index = 0;

    _children.foreach([&](auto &entry) {
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
    if (size == sizeof(HjDirEntry))
    {
        size_t index = handle.offset() / sizeof(HjDirEntry);

        FileListing *listing = (FileListing *)handle.attached;

        if (index < listing->count)
        {
            *((HjDirEntry *)buffer) = listing->entries[index];

            return sizeof(HjDirEntry);
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

    _children.foreach([&](auto &entry) {
        if (entry.name == name)
        {
            result = entry.node;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    return result;
}

HjResult FsDirectory::link(String name, RefPtr<FsNode> child)
{
    if (find(name))
    {
        return ERR_FILE_EXISTS;
    }

    _children.push_back({name, child});

    return SUCCESS;
}

HjResult FsDirectory::unlink(String name)
{

    bool has_removed_an_entry = _children.remove_all_match(
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
