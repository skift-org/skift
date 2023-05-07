#pragma once

#include <karm-hash/hash.h>
#include <karm-io/traits.h>

// https://gist.github.com/vurtun/760a6a2a198b706a7b1a6197aa5ac747
// https://bitbucket.org/rmitton/tigr/src/be3832bee7fb2f274fe5823e38f8ec7fa94e0ce9/src/tigr_inflate.c?at=default&fileviewer=file-view-default
// https://github.com/github/putty/blob/49fb598b0e78d09d6a2a42679ee0649df482090e/sshzlib.c
// https://www.ietf.org/rfc/rfc1951.txt

namespace Deflate {

struct Compressor : public Io::Writer {
};

struct Decompressor : public Io::Reader {
};

} // namespace Deflate
