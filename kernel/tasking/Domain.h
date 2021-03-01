#pragma once

#include <libutils/Path.h>

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

    RefPtr<FsNode> find(Path path);

    ResultOr<RefPtr<FsHandle>> open(Path path, OpenFlag flags);

    ResultOr<RefPtr<FsHandle>> connect(Path path);

    Result link(Path path, RefPtr<FsNode> node);

    Result unlink(Path path);

    Result rename(Path old_path, Path new_path);

    Result mkdir(Path path);

    Result mkpipe(Path path);

    Result mklink(Path old_path, Path new_path);
};