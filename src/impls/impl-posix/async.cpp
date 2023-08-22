#include <karm-async/async.h>
#include <karm-sys/proc.h>

namespace Karm::Async::_Embed {

struct PosixLoop : public Loop {
    TimeStamp _now;

    PosixLoop(TimeStamp now) : _now(now) {}

    TimeStamp now() override {
        return _now;
    }

    Res<> wait(TimeStamp until) override {
        try$(Sys::sleepUntil(until));
        _now = Sys::now();
        return Ok();
    }
};

static Opt<PosixLoop> _loop;
Loop &loop() {
    if (not _loop) {
        _loop.emplace(Sys::now());
    }
    return *_loop;
}

} // namespace Karm::Async::_Embed
