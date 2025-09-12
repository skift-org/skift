export module Karm.Debug;

import Karm.Core;

namespace Karm::Debug {

export struct Flag;

static Flag* _first = nullptr;

export struct Flag : Meta::Pinned {
    Str name;
    bool enabled = false;
    Flag* next = nullptr;

    Flag(Str name, bool enabled = false) : name(name), enabled(enabled) {
        if (_first)
            next = _first;
        _first = this;
    }

    operator bool() const {
        return enabled;
    };
};

export Res<> enable(Str flag) {
    bool ok = false;
    for (auto* f = _first; f; f = f->next) {
        if (Glob::matchGlob(flag, f->name)) {
            f->enabled = true;
            ok = true;
        }
    }
    if (not ok)
        return Error::invalidInput("no such flag");
    return Ok();
}

export Vec<Str> flags() {
    Vec<Str> res;
    for (auto* f = _first; f; f = f->next) {
        res.pushBack(f->name);
    }
    return res;
}

} // namespace Karm::Debug