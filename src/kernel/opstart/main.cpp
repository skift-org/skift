#include <karm/entry>

import Karm.Logger;
import Opstart;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    logInfo("opstart " stringify$(__ck_version_value));

    auto file = co_try$(Sys::File::open("file:/loader.json"_url));
    auto fileStr = co_try$(Io::readAllUtf8(file));
    auto json = co_try$(Json::parse(fileStr));

    logInfo("configs: {}", json);
    auto configs = co_try$(Opstart::Configs::fromJson(json));

    if (configs.entries.len() > 1 or configs.entries.len() == 0)
        co_return co_await Opstart::showMenuAsync(env, configs, ct);
    else
        co_try$(splashScreen(configs.entries[0]));

    co_return Opstart::loadEntry(configs.entries[0]);
}
