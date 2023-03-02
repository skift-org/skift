#pragma once

#include <hjert-api/api.h>
#include <karm-base/array.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>

namespace Hjert::Core {

struct _Object {
};

template <typename Crtp>
struct Object : public _Object {
    using _Crtp = Crtp;
};

using Slot = OptStrong<_Object>;

struct Domain : public Object<Domain> {
    Array<Slot, 4096> _slots;

    static Res<Strong<Domain>> create();

    Res<Hj::Cap> add(Hj::Cap dest, Strong<_Object> obj);
};

} // namespace Hjert::Core
