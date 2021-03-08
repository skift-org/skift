#pragma once

#include <skift/Time.h>

#include <libutils/Vector.h>

#include "kernel/node/Handle.h"
#include "kernel/system/System.h"

struct Task;

class Blocker
{
public:
private:
    Result _result = SUCCESS;
    TimeStamp _timeout = -1;

public:
    Result result() { return _result; }

    void timeout(TimeStamp ts) { _timeout = ts; }

    virtual ~Blocker() {}

    void unblock(Task &task)
    {
        _result = SUCCESS;
        on_unblock(task);
    }

    void timeout(Task &task)
    {
        _result = TIMEOUT;
        on_timeout(task);
    }

    void interrupt(Task &task, Result result)
    {
        _result = result;
        on_interrupt(task);
    }

    bool has_timeout()
    {
        return _timeout != (Timeout)-1 && _timeout <= system_get_tick();
    }

    virtual bool can_unblock(Task &) { return true; }

    virtual void on_unblock(Task &) {}

    virtual void on_timeout(Task &) {}

    virtual void on_interrupt(Task &) {}
};

class BlockerAccept : public Blocker
{
private:
    RefPtr<FsNode> _node;

public:
    BlockerAccept(RefPtr<FsNode> node) : _node(node)
    {
    }

    bool can_unblock(Task &task) override;

    void on_unblock(Task &task) override;
};

class BlockerConnect : public Blocker
{
private:
    RefPtr<FsNode> _connection;

public:
    BlockerConnect(RefPtr<FsNode> connection)
        : _connection(connection)
    {
    }

    bool can_unblock(Task &task) override;
};

class BlockerRead : public Blocker
{
private:
    FsHandle &_handle;

public:
    BlockerRead(FsHandle &handle)
        : _handle{handle}
    {
    }

    bool can_unblock(Task &task) override;

    void on_unblock(Task &task) override;
};

struct Selected
{
    int handle_index;
    RefPtr<FsHandle> handle;
    PollEvent events;
};

class BlockerSelect : public Blocker
{
private:
    Vector<Selected> &_handles;
    Optional<Selected> _selected;

public:
    Optional<Selected> selected() { return _selected; }

    BlockerSelect(Vector<Selected> &handles)
        : _handles{handles}
    {
    }

    bool can_unblock(Task &task) override;

    void on_unblock(Task &task) override;
};

class BlockerTime : public Blocker
{
private:
    Tick _wakeup_tick;

public:
    BlockerTime(Tick wakeup_tick)
        : _wakeup_tick(wakeup_tick)
    {
    }

    bool can_unblock(Task &task) override;
};

class BlockerWait : public Blocker
{
private:
    Task *_task;
    int *_exit_value;

public:
    BlockerWait(Task *task, int *exit_value)
        : _task(task), _exit_value(exit_value)
    {
    }

    bool can_unblock(Task &task) override;

    void on_unblock(Task &task) override;
};

class BlockerWrite : public Blocker
{
private:
    FsHandle &_handle;

public:
    BlockerWrite(FsHandle &handle)
        : _handle{handle}
    {
    }

    bool can_unblock(Task &task) override;

    void on_unblock(Task &task) override;
};
