#pragma once

#include <karm-base/range.h>
#include <karm-base/string.h>

#include "spec.h"

namespace Handover {

struct Builder {
    void *_buf{};
    size_t _size{};
    char *_string{};

    Builder(MutSlice<Byte> slice)
        : _buf(slice.buf()), _size(slice.len()), _string((char *)end(slice)) {
        payload() = {};
        payload().magic = COOLBOOT;
        payload().size = slice.len();
    }

    Payload &payload() {
        return *static_cast<Payload *>(_buf);
    }

    Record &record(size_t index) {
        return payload().records[index];
    }

    void add(Record r) {
        if (_size < sizeof(Record))
            return;

        for (size_t i = 0; i < payload().len; i++) {
            if (record(i).tag == r.tag &&
                record(i).end() == r.start &&
                shouldMerge(r.tag)) {
                record(i).size += r.size;
                return;
            }
        }

        record(payload().len) = r;
        payload().len++;
    }

    void add(Tag tag, uint32_t flags = 0, USizeRange range = {}, uint64_t more = 0) {
        add({
            .tag = tag,
            .flags = flags,
            .start = range.start(),
            .size = range.size(),
            .more = more,
        });
    }

    size_t add(Str str) {
        _string -= str.len() + 1;
        memcpy(_string, str.buf(), str.len());
        _string[str.len()] = '\0';
        return _string - static_cast<char *>(_buf);
    }

    void agent(Str str) {
        payload().agent = add(str);
    }

    Payload &finalize() {
        return *(Payload *)_buf;
    }
};

} // namespace Handover
