#include <embed-sys/sys.h>

#include "info.h"

namespace Karm::Sys {

Res<SysInfo> sysinfo() {
    SysInfo infos;
    try$(Embed::populate(infos));
    return Ok(infos);
}

Res<MemInfo> meminfo() {
    MemInfo infos;
    try$(Embed::populate(infos));
    return Ok(infos);
}

Res<Vec<CpuInfo>> cpusinfo() {
    Vec<CpuInfo> infos;
    try$(Embed::populate(infos));
    return Ok(infos);
}

Res<UserInfo> userinfo() {
    UserInfo infos;
    try$(Embed::populate(infos));
    return Ok(infos);
}

Res<Vec<UserInfo>> usersinfo() {
    Vec<UserInfo> infos;
    try$(Embed::populate(infos));
    return Ok(infos);
}

} // namespace Karm::Sys
