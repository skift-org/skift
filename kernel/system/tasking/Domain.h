#pragma once

#include <libio/Path.h>

#include "system/node/Handle.h"
#include "system/node/Node.h"

struct Domain
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

    ResultOr<RefPtr<FsHandle>> open(IO::Path path, HjOpenFlag flags);

    ResultOr<RefPtr<FsHandle>> connect(IO::Path path);

    HjResult link(IO::Path path, RefPtr<FsNode> node);

    HjResult unlink(IO::Path path);

    HjResult rename(IO::Path old_path, IO::Path new_path);

    HjResult mkdir(IO::Path path);

    HjResult mkpipe(IO::Path path);

    HjResult mklink(IO::Path old_path, IO::Path new_path);
};