#pragma once

#include <karm-mime/url.h>

namespace Karm::Sys {

enum struct Type {
    FILE,
    DIR,
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

Res<Stat> stat(Mime::Url const& url);

static inline Res<bool> isFile(Mime::Url const& url) {
    return Ok(try$(stat(url)).type == Type::FILE);
}

static inline Res<bool> isDir(Mime::Url const& url) {
    return Ok(try$(stat(url)).type == Type::DIR);
}

} // namespace Karm::Sys
