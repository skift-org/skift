#pragma once

#include <libio/Path.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Node.h"

class Domain
{
private:
    RefPtr<FsNode> _root;

public:
    RefPtr<FsNode> root() { return _root; }

    Domain();

    Domain(const Domain &other);

    ~Domain();

    Domain &operator=(const Domain &other);

    RefPtr<FsNode> find(IO::Path path);

    ResultOr<RefPtr<FsHandle>> open(IO::Path path, OpenFlag flags);

    ResultOr<RefPtr<FsHandle>> connect(IO::Path path);

    Result link(IO::Path path, RefPtr<FsNode> node);

    Result unlink(IO::Path path);

    Result rename(IO::Path old_path, IO::Path new_path);

    Result mkdir(IO::Path path);

    Result mkpipe(IO::Path path);

    Result mklink(IO::Path old_path, IO::Path new_path);
};