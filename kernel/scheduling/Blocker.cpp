#include "kernel/scheduling/Blocker.h"
#include "kernel/tasking/Task.h"

/* --- BlockerAccept -------------------------------------------------------- */

bool BlockerAccept::can_unblock(Task &)
{
    return !_node->is_acquire() && _node->can_accept();
}

void BlockerAccept::on_unblock(Task &task)
{
    _node->acquire(task.id);
}

/* --- BlockerConnect ------------------------------------------------------- */

bool BlockerConnect::can_unblock(Task &)
{
    return _connection->is_accepted();
}

/* --- BlockerRead ---------------------------------------------------------- */

bool BlockerRead::can_unblock(Task &)
{
    return !_handle.node()->is_acquire() && _handle.node()->can_read(_handle);
}

void BlockerRead::on_unblock(Task &task)
{
    _handle.node()->acquire(task.id);
}

/* --- BlockerSelect -------------------------------------------------------- */

bool BlockerSelect::can_unblock(Task &)
{
    for (size_t i = 0; i < _handles.count(); i++)
    {
        auto &selected = _handles[i];

        if (selected.handle->poll(selected.events) != 0)
        {
            return true;
        }
    }

    return false;
}

void BlockerSelect::on_unblock(Task &)
{
    for (size_t i = 0; i < _handles.count(); i++)
    {
        auto &el = _handles[i];

        PollEvent events = el.handle->poll(el.events);

        if (events != 0)
        {
            _selected = el;
            _selected->events = events;
            return;
        }
    }
}

/* --- BlockerTime ---------------------------------------------------------- */

bool BlockerTime::can_unblock(Task &)
{
    return system_get_tick() >= _wakeup_tick;
}

/* --- BlockerWait ---------------------------------------------------------- */

bool BlockerWait::can_unblock(Task &)
{
    return _task->state() == TASK_STATE_CANCELED;
}

void BlockerWait::on_unblock(Task &)
{
    *_exit_value = _task->exit_value;
}

/* --- BlockerWrite ---------------------------------------------------------- */

bool BlockerWrite::can_unblock(Task &)
{
    return !_handle.node()->is_acquire() &&
           _handle.node()->can_write(_handle);
}

void BlockerWrite::on_unblock(Task &task)
{
    _handle.node()->acquire(task.id);
}
