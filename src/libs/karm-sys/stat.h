#pragma once

#include <karm-core/macros.h>

import Karm.Core;
import Karm.Ref;

namespace Karm::Sys {

enum struct Type {
    FILE,
    DIR,
    OTHER,
};

struct Stat {
    Type type;
    usize size;
    SystemTime accessTime;
    SystemTime modifyTime;
    SystemTime changeTime;

    bool operator==(Type other) const {
        return type == other;
    }
};

Res<Stat> stat(Ref::Url const& url);

static inline Res<bool> isFile(Ref::Url const& url) {
    return Ok(try$(stat(url)).type == Type::FILE);
}

static inline Res<bool> isDir(Ref::Url const& url) {
    return Ok(try$(stat(url)).type == Type::DIR);
}

} // namespace Karm::Sys
