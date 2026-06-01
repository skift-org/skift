module;

#include <hal/mem.h>
#include <hal/pmm.h>
#include <karm/macros>

export module Hjert.Core:job;

import Karm.Core;

import :object;

using namespace Karm;

namespace Hjert::Core {

export struct Job : BaseObject<Job, Hj::Type::JOB> {
    Opt<Arc<Job>> _parent = NONE;
    Vec<Object*> _children;

    static Res<Arc<Job>> createRoot() {
        return Ok(makeArc<Job>(NONE));
    }

    static Res<Arc<Job>> create(Arc<Job> parent) {
        return Ok(makeArc<Job>(parent));
    }

    Job(None) {}

    Job(Arc<Job> parent) : _parent(parent) {
        ObjectLockScope _{*parent};
        if (parent->_signals.has(Hj::Sigs::EXITED))
            _signals.set(Hj::Sigs::EXITED);
        parent->_children.pushBack(this);
    }

    ~Job() {
        if (auto& [parent] = _parent)
            parent->removeChild(this);
    }

    void addChild(Object* child) {
        ObjectLockScope _{*this};
        if (_signals.has(Hj::Sigs::EXITED))
            child->signal({Hj::Sigs::EXITED}, {});
        _children.pushBack(child);
    }

    void removeChild(Object* child) {
        ObjectLockScope _{*this};
        _children.removeAll(child);
    }

    void _signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) override {
        if (set.has(Hj::Sigs::EXITED) and not _signals.has(Hj::Sigs::EXITED))
            for (auto child : _children)
                child->signal({Hj::Sigs::EXITED}, {});

        BaseObject::_signalUnlock(set, unset);
    }
};

} // namespace Hjert::Core
