#include <karm-logger/logger.h>

#include "_embed.h"

#include "async.h"

namespace Karm::Sys {

/* --- Sink --- */

void Sched::_post(Sink &sink, Box<Event> event) {
    _queued.emplaceBack(&sink, std::move(event));
}

void Sched::_move(Sink &from, Sink *to) {
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

void Sched::_bind(Source &source, Sink &sink) {
    _sources.emplaceBack(&source, &sink);
}

void Sched::_move(Source &from, Source *to) {
    for (auto &s : _sources) {
        if (s.source == &from) {
            s.source = to;
        }
    }
}

/* --- Public --- */

void Sched::_collect() {
    for (usize i = 0; i < _sources.len(); i++) {
        auto [source, sink] = _sources[i];
        if (not source or not sink) {
            _sources.removeAt(i--);
        }
    }
}

Res<TimeStamp> Sched::poll() {
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

Res<usize> Sched::dispatch() {
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

Res<> Sched::run() {
    while (true) {
        auto until = try$(runOnce());
        if (exited())
            return takeResult();
        try$(wait(until));
    }
}

Res<TimeStamp> Sched::runOnce() {
    usize count = 0;
    TimeStamp until = TimeStamp::endOfTime();

    do {
        until = try$(poll());
        count = try$(dispatch());
        _collect();
    } while (count);

    return Ok(until);
}

Sched &globalSched() {
    return _Embed::globalSched();
}

} // namespace Karm::Sys
