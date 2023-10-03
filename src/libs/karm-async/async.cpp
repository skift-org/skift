#include <karm-logger/logger.h>

#include "async.h"

namespace Karm::Async {

/* --- Sink --- */

void Loop::_post(Sink &sink, Box<Event> event) {
    _queued.emplaceBack(&sink, std::move(event));
}

void Loop::_move(Sink &from, Sink *to) {
    for (auto &q : _queued) {
        if (q.sink == &from) {
            q.sink = to;
        }
    }

    for (auto &s : _sources) {
        if (s.sink == &from) {
            s.sink = to;
        }
    }
}

/* --- Source --- */

void Loop::_bind(Source &source, Sink &sink) {
    _sources.emplaceBack(&source, &sink);
}

void Loop::_move(Source &from, Source *to) {
    for (auto &s : _sources) {
        if (s.source == &from) {
            s.source = to;
        }
    }
}

/* --- Public --- */

void Loop::_collect() {
    for (usize i = 0; i < _sources.len(); i++) {
        auto [source, sink] = _sources[i];
        if (not source or not sink) {
            _sources.removeAt(i--);
        }
    }
}

Res<TimeStamp> Loop::poll() {
    TimeStamp until = TimeStamp::endOfTime();

    for (usize i = 0; i < _sources.len(); i++) {
        auto [source, sink] = _sources[i];
        if (not source or not sink)
            continue;

        auto ts = try$(source->poll(*sink));

        if (ts < until)
            until = ts;
    }

    return Ok(until);
}

Res<usize> Loop::dispatch() {
    usize count = 0;
    for (usize i = 0; i < _queued.len(); i++) {
        count++;
        auto *target = _queued[i].sink;
        auto event = std::move(_queued[i].event);

        if (not target)
            continue;

        auto res = target->post(*event);
        if (not res) {
            _queued.removeRange(0, i);
            return res.none();
        }
    }

    _queued.clear();
    return Ok(count);
}

Res<> Loop::run() {
    while (true) {
        usize count = 0;
        TimeStamp until = TimeStamp::endOfTime();

        do {
            until = try$(poll());
            count = try$(dispatch());
            _collect();
        } while (count);

        if (_sources.len() == 0)
            return Ok();

        if (_ret) {
            auto ret = *_ret;
            _ret = NONE;
            return *_ret;
        }

        try$(wait(until));
    }
}

} // namespace Karm::Async
