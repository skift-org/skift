#pragma once

#include <karm-mime/url.h>

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

Res<Stat> stat(Mime::Url const &url);

static inline Res<bool> isFile(Mime::Url const &url) {
    return Ok(try$(stat(url)).type == Stat::FILE);
}

static inline Res<bool> isDir(Mime::Url const &url) {
    return Ok(try$(stat(url)).type == Stat::DIR);
}

} // namespace Karm::Sys
