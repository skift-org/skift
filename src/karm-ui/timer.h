#pragma once

#include <karm-sys/time.h>
#include <karm-time/time.h>

#include "react.h"

namespace Karm::Ui {

struct _Timer : public React<_Timer> {
    struct Wrap {
        _Timer &_t;

        Wrap(_Timer &t) : _t(t) {}

        void fireAt(Time::Stamp stamp) {
            _t._deadline = stamp;
            _t._interval = Time::Span::infinite();
        }

        void fireNow() {
            fireAt(Sys::now());
        }

        void fireOnce(Time::Span span) {
            fireAt(Sys::now() + span);
        }

        void fireRepeatedly(Time::Span span) {
            fireOnce(span);
            _t._interval = span;
        }

        void cancel() {
            _t._deadline = Time::Stamp::endOfTime();
            _t._interval = Time::Span::infinite();
        }
    };

    Time::Span _interval;
    Time::Stamp _deadline;
    Func<Child(Wrap)> _build;

    void fire() {
        if (_deadline.isEndOfTime()) {
            return;
        }
        _deadline += _interval;
        shouldRebuild();
    }

    Child build() override {
        return _build(Wrap(*this));
    }
};

using Timer = typename _Timer::Wrap;

} // namespace Karm::Ui
