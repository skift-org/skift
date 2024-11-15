#include "info.h"

#include "_embed.h"

#include "proc.h"

namespace Karm::Sys {

Res<SysInfo> sysinfo() {
    try$(ensureUnrestricted());
    SysInfo infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

Res<MemInfo> meminfo() {
    MemInfo infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

Res<Vec<CpuInfo>> cpusinfo() {
    Vec<CpuInfo> infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

Res<UserInfo> userinfo() {
    try$(ensureUnrestricted());
    UserInfo infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

Res<Vec<UserInfo>> usersinfo() {
    try$(ensureUnrestricted());
    Vec<UserInfo> infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

} // namespace Karm::Sys
