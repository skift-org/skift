#pragma once

#include <url/url.h>

namespace Karm::Sys {

struct Stat {
    enum Type {
        FILE,
        DIR,
    } type;
    usize size;
    TimeStamp accessTime;
    TimeStamp modifyTime;
    TimeStamp changeTime;
};

Res<Stat> stat(Url::Url const &url);

static inline Res<bool> isFile(Url::Url const &url) {
    return Ok(try$(stat(url)).type == Stat::FILE);
}

static inline Res<bool> isDir(Url::Url const &url) {
    return Ok(try$(stat(url)).type == Stat::DIR);
}

} // namespace Karm::Sys
