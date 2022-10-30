#pragma once

#include <karm-base/time.h>
#include <karm-sys/time.h>

#include "react.h"

namespace Karm::Ui {

struct _Timer : public React<_Timer> {
    struct Wrap {
        _Timer &_t;

        Wrap(_Timer &t) : _t(t) {}

        void fireAt(TimeStamp stamp) {
            _t._deadline = stamp;
            _t._interval = TimeSpan::infinite();
        }

        void fireNow() {
            fireAt(Sys::now());
        }

        void fireOnce(TimeSpan span) {
            fireAt(Sys::now() + span);
        }

        void fireRepeatedly(TimeSpan span) {
            fireOnce(span);
            _t._interval = span;
        }

        void cancel() {
            _t._deadline = TimeStamp::endOfTime();
            _t._interval = TimeSpan::infinite();
        }
    };

    TimeSpan _interval;
    TimeStamp _deadline;
    Func<Child(Wrap)> _build;

    void fire() {
        if (_deadline.isEndOfTime()) {
            return;
        }
        _deadline += _interval;
        shouldRebuild(*this);
    }

    Child build() override {
        return _build(Wrap(*this));
    }
};

using Timer = typename _Timer::Wrap;

} // namespace Karm::Ui
