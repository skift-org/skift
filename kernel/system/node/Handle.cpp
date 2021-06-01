
#include <assert.h>
#include <string.h>

#include <abi/Result.h>
#include <libmath/MinMax.h>

#include "system/node/Connection.h"
#include "system/node/Handle.h"
#include "system/scheduling/Blocker.h"
#include "system/scheduling/Scheduler.h"

FsHandle::FsHandle(RefPtr<FsNode> node, HjOpenFlag flags)
{
    _node = node;
    _flags = flags;
    _offset = 0;

    node->ref_handle(*this);

    node->acquire(scheduler_running_id());
    node->open(*this);
    node->release(scheduler_running_id());
}

FsHandle::FsHandle(FsHandle &other)
{
    _node = other.node();
    _flags = other.flags();
    _offset = other.offset();

    _node->ref_handle(*this);

    _node->acquire(scheduler_running_id());
    _node->open(*this);
    _node->release(scheduler_running_id());
}

FsHandle::~FsHandle()
{
    _node->acquire(scheduler_running_id());
    _node->close(*this);
    _node->release(scheduler_running_id());

    _node->deref_handle(*this);
}

bool FsHandle::locked()
{
    return _lock.locked();
}

void FsHandle::acquire(int who_acquire)
{
    _lock.acquire_for(who_acquire);
}

void FsHandle::release(int who_release)
{
    _lock.release_for(who_release);
}

PollEvent FsHandle::poll(PollEvent events)
{
    PollEvent selected_events = 0;

    if ((events & POLL_READ) && _node->can_read(*this))
    {
        selected_events |= POLL_READ;
    }

    if ((events & POLL_WRITE) && _node->can_write(*this))
    {
        selected_events |= POLL_WRITE;
    }

    if ((events & POLL_CONNECT) && _node->is_accepted())
    {
        selected_events |= POLL_CONNECT;
    }

    if ((events & POLL_ACCEPT) && _node->can_accept())
    {
        selected_events |= POLL_ACCEPT;
    }

    return selected_events;
}

ResultOr<size_t> FsHandle::read(void *buffer, size_t size)
{
    if (!has_flag(HJ_OPEN_READ) &&
        !has_flag(HJ_OPEN_SERVER) &&
        !has_flag(HJ_OPEN_CLIENT))
    {
        return ERR_WRITE_ONLY_STREAM;
    }

    BlockerRead blocker{*this};

    TRY(task_block(scheduler_running(), blocker, -1));

    auto read_result = _node->read(*this, buffer, size);

    if (!read_result.success())
    {
        _node->release(scheduler_running_id());
        return read_result;
    }

    _offset += read_result.unwrap();
    _node->release(scheduler_running_id());

    return read_result;
}

ResultOr<size_t> FsHandle::write(const void *buffer, size_t size)
{
    if (!has_flag(HJ_OPEN_WRITE) &&
        !has_flag(HJ_OPEN_SERVER) &&
        !has_flag(HJ_OPEN_CLIENT))
    {
        return ERR_READ_ONLY_STREAM;
    }

    auto attempt_a_write = [&](const void *buffer, size_t size) -> ResultOr<size_t> {
        BlockerWrite blocker{*this};

        TRY(task_block(scheduler_running(), blocker, -1));

        if (has_flag(HJ_OPEN_APPEND))
        {
            _offset = _node->size();
        }

        auto write_result = _node->write(*this, buffer, size);

        if (!write_result.success())
        {
            _node->release(scheduler_running_id());

            return write_result;
        }

        _offset += write_result.unwrap();

        _node->release(scheduler_running_id());

        return write_result;
    };

    size_t written = 0;

    do
    {
        auto remaining = size - written;
        auto remaining_buffer = reinterpret_cast<const char *>((reinterpret_cast<uintptr_t>(buffer)) + written);

        written += TRY(attempt_a_write(remaining_buffer, remaining));
    } while (written < size);

    return written;
}

ResultOr<ssize64_t> FsHandle::seek(IO::SeekFrom from)
{
    _node->acquire(scheduler_running_id());
    size_t size = _node->size();
    _node->release(scheduler_running_id());

    switch (from.whence)
    {
    case IO::Whence::START:
        _offset = MAX(0, from.position);
        break;

    case IO::Whence::CURRENT:
        if (from.position < 0)
        {
            if ((size_t)-from.position <= _offset)
            {
                _offset = _offset + from.position;
            }
            else
            {
                _offset = 0;
            }
        }
        else
        {
            _offset = _offset + from.position;
        }
        break;

    case IO::Whence::END:
        if (from.position < 0)
        {
            if ((size_t)-from.position <= size)
            {
                _offset = size + from.position;
            }
            else
            {
                _offset = 0;
            }
        }
        else
        {
            _offset = size + from.position;
        }

        break;

    default:
        return ERR_INVALID_ARGUMENT;
    }

    return _offset;
}

HjResult FsHandle::call(IOCall request, void *args)
{
    _node->acquire(scheduler_running_id());
    HjResult result = _node->call(*this, request, args);
    _node->release(scheduler_running_id());

    return result;
}

HjResult FsHandle::stat(HjStat *stat)
{
    _node->acquire(scheduler_running_id());
    stat->size = _node->size();
    stat->type = _node->type();
    _node->release(scheduler_running_id());

    return SUCCESS;
}

ResultOr<RefPtr<FsHandle>> FsHandle::accept()
{
    BlockerAccept blocker{_node};
    TRY(task_block(scheduler_running(), blocker, -1));

    auto connection_or_result = _node->accept();

    if (!connection_or_result.success())
    {
        _node->release(scheduler_running_id());
        return connection_or_result.result();
    }

    // We need to create the fshandle before releasing the lock
    // Because this will increment FsNode::servers

    auto connection = connection_or_result.unwrap();

    auto connection_handle = make<FsHandle>(connection, HJ_OPEN_SERVER);

    _node->release(scheduler_running_id());

    return connection_handle;
}
