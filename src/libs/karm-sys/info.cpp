#include <embed/sys.h>

#include "info.h"

namespace Karm::Sys {

Result<SysInfo> sysinfo() {
    SysInfo infos;
    try$(Embed::populate(infos));
    return infos;
}

Result<MemInfo> meminfo() {
    MemInfo infos;
    try$(Embed::populate(infos));
    return infos;
}

Result<Vec<CpuInfo>> cpusinfo() {
    Vec<CpuInfo> infos;
    try$(Embed::populate(infos));
    return infos;
}

Result<UserInfo> userinfo() {
    UserInfo infos;
    try$(Embed::populate(infos));
    return infos;
}

Result<Vec<UserInfo>> usersinfo() {
    Vec<UserInfo> infos;
    try$(Embed::populate(infos));
    return infos;
}

} // namespace Karm::Sys
