#pragma once

import Karm.Core;

#include <karm-logger/logger.h>

#include "spec.h"

template <>
struct Karm::Io::Formatter<Handover::Record> {
    Res<> format(Io::TextWriter& writer, Handover::Record record) {
        return Io::format(writer, "Record({}, {x}-{x})", record.name(), record.start, record.end());
    }
};

namespace Handover {

inline urange rangeOf(Record record) {
    return {record.start, record.size};
}

inline bool colidesWith(Record record, Record other) {
    return rangeOf(record).overlaps(rangeOf(other));
}

inline Pair<Record, Record> split(Record record, Record other) {
    auto [lower, upper] = rangeOf(record).split(rangeOf(other));
    Record lowerRecord = record;
    Record upperRecord = record;
    lowerRecord.start = lower.start;
    lowerRecord.size = lower.size;
    upperRecord.start = upper.start;
    upperRecord.size = upper.size;
    return {lowerRecord, upperRecord};
}

struct Builder {
    void* _buf{};
    usize _size{};
    char* _string{};
    _Vec<ViewBuf<Record>> _records;

    Builder(MutSlice<u8> slice)
        : _buf(slice.buf()),
          _size(slice.len()),
          _string((char*)end(slice)),
          _records(ViewBuf<Record>{(Manual<Record>*)payload().records, _size / sizeof(Record)}) {
        payload() = {};
        payload().magic = COOLBOOT;
        payload().size = slice.len();
    }

    Payload& payload() {
        return *static_cast<Payload*>(_buf);
    }

    void add(Record record) {
        if (record.size == 0)
            return;

        for (usize i = 0; i < _records.len(); i++) {
            auto other = _records[i];

            // Merge with previous
            if (other.tag == record.tag and
                other.end() == record.start and
                shouldMerge(record.tag)) {
                // logDebug("handover: merge {} with {}", record, other);
                _records.removeAt(i);
                other.size += record.size;
                add(other);
                return;
            }

            // Merge with next
            if (other.tag == record.tag and
                other.start == record.end() and
                shouldMerge(record.tag)) {
                // logDebug("handover: merge {} with {}", record, other);

                _records.removeAt(i);
                record.size += other.size;
                add(record);
                return;
            }

            if (colidesWith(record, other)) {
                if (shouldMerge(record.tag) and not shouldMerge(other.tag)) {
                    // logDebug("handover: splitting record {} with {}", record, other);

                    _records.removeAt(i);
                    auto [lower, upper] = split(record, other);

                    add(other);
                    add(lower);
                    add(upper);
                    return;
                } else if (not shouldMerge(record.tag) and shouldMerge(other.tag)) {
                    // logDebug("handover: splitting record {} with {}", other, record);

                    _records.removeAt(i);

                    auto [lower, upper] = split(other, record);
                    add(record);
                    add(lower);
                    add(upper);
                    return;
                } else {
                    logWarn("handover: record {} colides with {}", record, other);
                    return;
                }
            }

            if (other.start > record.start) {
                // logDebug("handover: insert {} at {}", record, i);
                _records.insert(i, record);
                return;
            }
        }

        // logDebug("handover: append {}", record);
        _records.pushBack(record);
    }

    void add(Tag tag, u32 flags = 0, urange range = {}, u64 more = 0) {
        add({
            .tag = tag,
            .flags = flags,
            .start = range.start,
            .size = range.size,
            .more = more,
        });
    }

    usize add(Str str) {
        _string -= str.len() + 1;
        std::memcpy(_string, str.buf(), str.len());
        _string[str.len()] = '\0';
        return _string - static_cast<char*>(_buf);
    }

    void agent(Str str) {
        payload().agent = add(str);
    }

    Payload& finalize() {
        payload().len = _records.len();
        return payload();
    }
};

} // namespace Handover
