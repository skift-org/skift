#pragma once

#include <karm-base/result.h>
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

Result<SysInfo> sysinfo();

struct MemInfo {
    size_t physicalTotal;
    size_t physicalUsed;

    size_t virtualTotal;
    size_t virtualUsed;

    size_t swapTotal;
    size_t swapUsed;
};

Result<MemInfo> meminfo();

struct CpuInfo {
    String name;
    String brand;
    String vendor;

    size_t usage;
    size_t freq;
};

Result<Vec<CpuInfo>> cpusinfo();

struct UserInfo {
    String name;
    String home;
    String shell;
};

Result<UserInfo> userinfo();

Result<Vec<UserInfo>> usersinfo();

} // namespace Karm::Sys
