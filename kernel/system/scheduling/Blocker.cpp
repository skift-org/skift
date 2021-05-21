#include "system/scheduling/Blocker.h"
#include "system/tasking/Task.h"

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
    bool should_be_unblock = false;

    for (size_t i = 0; i < _handles.count(); i++)
    {
        auto &selected = _handles[i];

        selected.result = selected.handle->poll(selected.events);

        if (selected.result != 0)
        {
            should_be_unblock = true;
        }
    }

    return should_be_unblock;
}

/* --- BlockerWait ---------------------------------------------------------- */

bool BlockerWait::can_unblock(Task &)
{
    return _task->state() == TASK_STATE_CANCELING;
}

void BlockerWait::on_unblock(Task &)
{
    _task->state(TASK_STATE_CANCELED);
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
