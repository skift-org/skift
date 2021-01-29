
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>

#include "kernel/node/Connection.h"
#include "kernel/node/Handle.h"
#include "kernel/scheduling/Blocker.h"
#include "kernel/scheduling/Scheduler.h"

FsHandle::FsHandle(RefPtr<FsNode> node, OpenFlag flags)
{
    _node = node;
    _flags = flags;
    _offset = 0;

    node->ref_handle(*this);

    node->acquire(scheduler_running_id());
    node->open(this);
    node->release(scheduler_running_id());
}

FsHandle::FsHandle(FsHandle &other)
{
    _node = other.node();
    _flags = other.flags();
    _offset = other.offset();

    _node->ref_handle(*this);

    _node->acquire(scheduler_running_id());
    _node->open(this);
    _node->release(scheduler_running_id());
}

FsHandle::~FsHandle()
{
    _node->acquire(scheduler_running_id());
    _node->close(this);
    _node->release(scheduler_running_id());

    _node->deref_handle(*this);
}

bool FsHandle::locked()
{
    return _lock.locked();
}

void FsHandle::acquire(int who_acquire)
{
    _lock.acquire_for(who_acquire, SOURCE_LOCATION);
}

void FsHandle::release(int who_release)
{
    _lock.release_for(who_release, SOURCE_LOCATION);
}

PollEvent FsHandle::poll(PollEvent events)
{
    PollEvent selected_events = 0;

    if ((events & POLL_READ) && _node->can_read(this))
    {
        selected_events |= POLL_READ;
    }

    if ((events & POLL_WRITE) && _node->can_write(this))
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
    if (!has_flag(OPEN_READ) &&
        !has_flag(OPEN_SERVER) &&
        !has_flag(OPEN_CLIENT))
    {
        return ERR_WRITE_ONLY_STREAM;
    }

    task_block(scheduler_running(), new BlockerRead(this), -1);

    auto result_or_read = _node->read(*this, buffer, size);

    if (result_or_read)
    {
        _offset += *result_or_read;
    }

    _node->release(scheduler_running_id());

    return result_or_read;
}

ResultOr<size_t> FsHandle::write(const void *buffer, size_t size)
{
    if (!has_flag(OPEN_WRITE) &&
        !has_flag(OPEN_SERVER) &&
        !has_flag(OPEN_CLIENT))
    {
        return ERR_READ_ONLY_STREAM;
    }

    auto attemp_a_write = [&](const void *buffer, size_t size) {
        task_block(scheduler_running(), new BlockerWrite(this), -1);

        if (has_flag(OPEN_APPEND))
        {
            _offset = _node->size();
        }

        auto result_or_written = _node->write(*this, buffer, size);

        if (result_or_written)
        {
            _offset += *result_or_written;
        }

        _node->release(scheduler_running_id());

        return result_or_written;
    };

    size_t written = 0;

    do
    {
        auto remaining = size - written;
        auto remaining_buffer = reinterpret_cast<const char *>((reinterpret_cast<uintptr_t>(buffer)) + written);

        auto result_or_written = attemp_a_write(remaining_buffer, remaining);

        if (result_or_written)
        {
            written += *result_or_written;
        }
        else
        {
            return result_or_written;
        }
    } while (written < size);

    return written;
}

Result FsHandle::seek(int offset, Whence whence)
{
    _node->acquire(scheduler_running_id());
    size_t size = _node->size();
    _node->release(scheduler_running_id());

    switch (whence)
    {
    case WHENCE_START:
        _offset = MAX(0, offset);
        break;

    case WHENCE_HERE:
        _offset = _offset + offset;
        break;

    case WHENCE_END:
        if (offset < 0)
        {
            if ((size_t)-offset <= size)
            {
                _offset = size + offset;
            }
            else
            {
                _offset = 0;
            }
        }
        else
        {
            _offset = size + offset;
        }

        break;
    default:
        logger_error("Invalid whence %d", whence);
        ASSERT_NOT_REACHED();
    }

    return SUCCESS;
}

ResultOr<int> FsHandle::tell(Whence whence)
{
    _node->acquire(scheduler_running_id());
    size_t size = _node->size();
    _node->release(scheduler_running_id());

    if (whence == WHENCE_START ||
        whence == WHENCE_HERE)
    {
        return _offset;
    }
    else if (whence == WHENCE_END)
    {
        return _offset - size;
    }
    else
    {
        ASSERT_NOT_REACHED();
    }
}

Result FsHandle::call(IOCall request, void *args)
{
    _node->acquire(scheduler_running_id());
    Result result = _node->call(*this, request, args);
    _node->release(scheduler_running_id());

    return result;
}

Result FsHandle::stat(FileState *stat)
{
    _node->acquire(scheduler_running_id());
    stat->size = _node->size();
    stat->type = _node->type();
    _node->release(scheduler_running_id());

    return SUCCESS;
}

ResultOr<FsHandle *> FsHandle::accept()
{
    task_block(scheduler_running(), new BlockerAccept(_node), -1);

    auto connection_or_result = _node->accept();

    if (!connection_or_result.success())
    {
        _node->release(scheduler_running_id());
        return connection_or_result.result();
    }

    // We need to create the fshandle before releasing the lock
    // Because this will increment FsNode::servers

    auto connection = connection_or_result.take_value();

    auto connection_handle = new FsHandle(connection, OPEN_SERVER);

    _node->release(scheduler_running_id());

    return connection_handle;
}
