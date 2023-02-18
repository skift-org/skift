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
    size_t physicalTotal;
    size_t physicalUsed;

    size_t virtualTotal;
    size_t virtualUsed;

    size_t swapTotal;
    size_t swapUsed;
};

Res<MemInfo> meminfo();

struct CpuInfo {
    String name;
    String brand;
    String vendor;

    size_t usage;
    size_t freq;
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
