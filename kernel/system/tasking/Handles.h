#pragma once

#include <libio/Path.h>

#include "system/node/Handle.h"

struct Handles
{
private:
    Lock _lock{"handles-lock"};

    RefPtr<FsHandle> _handles[PROCESS_HANDLE_COUNT];

    ResultOr<int> add(RefPtr<FsHandle> handle);

    HjResult add_at(RefPtr<FsHandle> handle, int index);

    bool is_valid_handle(int handle);

    HjResult remove(int handle_index);

    RefPtr<FsHandle> acquire(int handle_index);

    HjResult release(int handle_index);

public:
    Handles() {}

    ~Handles() { close_all(); }

    ResultOr<int> open(Domain &domain, IO::Path &path, HjOpenFlag flags);

    ResultOr<int> connect(Domain &domain, IO::Path &socket_path);

    HjResult close(int handle_index);

    void close_all();

    HjResult reopen(int handle, int *reopened);

    HjResult copy(int source, int destination);

    HjResult poll(HandlePoll *handles, size_t count, Timeout timeout);

    ResultOr<size_t> read(int handle_index, void *buffer, size_t size);

    ResultOr<size_t> write(int handle_index, const void *buffer, size_t size);

    ResultOr<ssize64_t> seek(int handle_index, IO::SeekFrom from);

    HjResult call(int handle_index, IOCall request, void *args);

    HjResult stat(int handle_index, HjStat *stat);

    ResultOr<int> accept(int handle_index);

    HjResult duplex(
        RefPtr<FsNode> node,
        int *server, HjOpenFlag server_flags,
        int *client, HjOpenFlag client_flags);

    HjResult term(int *server, int *client);

    HjResult pipe(int *reader, int *writer);

    HjResult pass(Handles &handles, int source, int destination);
};
