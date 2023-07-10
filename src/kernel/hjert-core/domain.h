#pragma once

#include "object.h"

namespace Hjert::Core {

struct Domain : public BaseObject<Domain, Hj::Type::DOMAIN> {
    using Slot = Opt<Strong<Object>>;

    static constexpr usize SHIFT = 11;
    static constexpr usize LEN = 2 << 11;
    static constexpr usize MASK = LEN - 1;

    Array<Slot, LEN> _slots;

    static Res<Strong<Domain>> create();

    Res<Hj::Cap> add(Hj::Cap dest, Strong<Object> obj);

    Res<Strong<Object>> get(Hj::Cap cap);

    template <typename T>
    Res<Strong<T>> get(Hj::Cap cap) {
        auto obj = try$(get(cap));
        if (not obj.is<T>()) {
            return Error::invalidHandle("type missmatch");
        }
        return Ok(try$(obj.cast<T>()));
    }

    Res<> drop(Hj::Cap cap);
};

} // namespace Hjert::Core
