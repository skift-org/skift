#include "object.h"

namespace Hjert::Core {

Atomic<usize> Object::_counter = 0;

void Object::label(Str label) {
    LockScope scope(_lock);
    _label = String(label);
}

String Object::label() const {
    if (not _label)
        return "<unlabeled>"s;
    return *_label;
}

void Object::_signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    _signals |= set;
    _signals &= ~unset;
}

Flags<Hj::Sigs> Object::_pollUnlock() {
    return _signals;
}

void Object::signal(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    LockScope scope(_lock);
    _signalUnlock(set, unset);
}

Flags<Hj::Sigs> Object::poll() {
    LockScope scope(_lock);
    return _pollUnlock();
}

} // namespace Hjert::Core
