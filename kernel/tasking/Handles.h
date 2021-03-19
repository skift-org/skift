#pragma once

#include <libutils/Path.h>

#include "kernel/node/Handle.h"

class Handles
{
private:
    Lock _lock{"handles-lock"};

    RefPtr<FsHandle> _handles[PROCESS_HANDLE_COUNT];

    ResultOr<int> add(RefPtr<FsHandle> handle);

    Result add_at(RefPtr<FsHandle> handle, int index);

    bool is_valid_handle(int handle);

    Result remove(int handle_index);

    RefPtr<FsHandle> acquire(int handle_index);

    Result release(int handle_index);

public:
    Handles() {}

    ~Handles() { close_all(); }

    ResultOr<int> open(Domain &domain, Path &path, OpenFlag flags);

    ResultOr<int> connect(Domain &domain, Path &socket_path);

    Result close(int handle_index);

    void close_all();

    Result reopen(int handle, int *reopened);

    Result copy(int source, int destination);

    Result poll(HandlePoll *handles, size_t count, Timeout timeout);

    ResultOr<size_t> read(int handle_index, void *buffer, size_t size);

    ResultOr<size_t> write(int handle_index, const void *buffer, size_t size);

    ResultOr<ssize64_t> seek(int handle_index, IO::SeekFrom from);

    Result call(int handle_index, IOCall request, void *args);

    Result stat(int handle_index, FileState *stat);

    ResultOr<int> accept(int handle_index);

    Result duplex(
        RefPtr<FsNode> node,
        int *server, OpenFlag server_flags,
        int *client, OpenFlag client_flags);

    Result term(int *server, int *client);

    Result pipe(int *reader, int *writer);

    Result pass(Handles &handles, int source, int destination);
};
