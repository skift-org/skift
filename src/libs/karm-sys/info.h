#pragma once

#include <karm-base/res.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>

namespace Karm::Sys {

struct SysInfo {
    String sysName;
    String sysVersion;

    String kernelName;
    String kernelVersion;

    String hostname;
};

Res<SysInfo> sysinfo();

struct MemInfo {
    usize physicalTotal;
    usize physicalUsed;

    usize virtualTotal;
    usize virtualUsed;

    usize swapTotal;
    usize swapUsed;
};

Res<MemInfo> meminfo();

struct CpuInfo {
    String name;
    String brand;
    String vendor;

    usize usage;
    usize freq;
};

Res<Vec<CpuInfo>> cpusinfo();

struct UserInfo {
    String name;
    String home;
    String shell;
};

Res<UserInfo> userinfo();

Res<Vec<UserInfo>> usersinfo();

inline bool isSkift() {
#ifdef __osdk_sys_skift__
    return true;
#else
    return false;
#endif
}

} // namespace Karm::Sys
