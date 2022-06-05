#pragma once

#include <karm-base/string.h>

#include "handover.h"

namespace Handover {

struct Builder {
    void *_buf;
    size_t _size;
    char *_string;

    Builder(void *buf, size_t size)
        : _buf(buf), _size(size) {
        payload() = {};
        payload().magic = COOLBOOT;
        payload().size = size;
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

        record(payload().len) = r;
        payload().len++;
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
};

} // namespace Handover
