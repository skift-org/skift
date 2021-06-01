#pragma once

#include <skift/Time.h>

#include <libutils/Vector.h>

#include "system/node/Handle.h"
#include "system/system/System.h"

struct Task;

struct Blocker
{
private:
    HjResult _result = SUCCESS;
    TimeStamp _timeout = -1;
    bool _interrupted = false;

public:
    HjResult result() { return _result; }

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

    void interrupt(Task &task, HjResult result)
    {
        _interrupted = true;
        _result = result;
        on_interrupt(task);
    }

    bool has_timeout()
    {
        return _timeout != (Timeout)-1 && _timeout <= system_get_tick();
    }

    bool is_interrupted()
    {
        return _interrupted;
    }

    virtual bool can_unblock(Task &) { return true; }

    virtual void on_unblock(Task &) {}

    virtual void on_timeout(Task &) {}

    virtual void on_interrupt(Task &) {}
};

struct BlockerAccept : public Blocker
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

struct BlockerConnect : public Blocker
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

struct BlockerRead : public Blocker
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
    PollEvent result;
};

struct BlockerSelect : public Blocker
{
private:
    Vector<Selected> &_handles;

public:
    BlockerSelect(Vector<Selected> &handles)
        : _handles{handles}
    {
    }

    bool can_unblock(Task &task) override;
};

struct BlockerTime : public Blocker
{
public:
    BlockerTime() {}

    bool can_unblock(Task &) override { return false; }
};

struct BlockerWait : public Blocker
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

struct BlockerWrite : public Blocker
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
