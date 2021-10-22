#pragma once

#include <libio/Path.h>

#include "system/node/Handle.h"
#include "system/node/Node.h"

struct Domain
{
private:
    Ref<FsNode> _root;

public:
    Ref<FsNode> root() { return _root; }

    Domain();

    Domain(const Domain &other);

    ~Domain();

    Domain &operator=(const Domain &other);

    Ref<FsNode> find(IO::Path path);

    ResultOr<Ref<FsHandle>> open(IO::Path path, HjOpenFlag flags);

    ResultOr<Ref<FsHandle>> connect(IO::Path path);

    HjResult link(IO::Path path, Ref<FsNode> node);

    HjResult unlink(IO::Path path);

    HjResult rename(IO::Path old_path, IO::Path new_path);

    HjResult mkdir(IO::Path path);

    HjResult mkpipe(IO::Path path);

    HjResult mklink(IO::Path old_path, IO::Path new_path);
};