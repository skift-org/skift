#pragma once

import Karm.Core;
import Karm.Ref;

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
    Ref::Url home;
    Ref::Url shell;
};

Res<UserInfo> userinfo();

Res<Vec<UserInfo>> usersinfo();

} // namespace Karm::Sys
