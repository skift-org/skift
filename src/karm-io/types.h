#pragma once

#include <karm-base/result.h>

namespace Karm::Io {

using Result = Base::Result<size_t>;

struct Writer {
    virtual Result write(void const *data, size_t size) = 0;
};

struct Reader {
    virtual Result read(void *data, size_t size) = 0;
};

struct Sink : public Writer {
    Result write(void const *, size_t size) { return size; }
};

struct Zero : public Reader {
    Result read(void *data, size_t size) {
        memset(data, 0, size);
        return size;
    }
};

} // namespace Karm::Io
