#include "info.h"

#include "_embed.h"

namespace Karm::Sys {

Res<SysInfo> sysinfo() {
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
    UserInfo infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

Res<Vec<UserInfo>> usersinfo() {
    Vec<UserInfo> infos;
    try$(_Embed::populate(infos));
    return Ok(infos);
}

} // namespace Karm::Sys
