#include <karm-sys/entry.h>

import Karm.Ml;

using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context&) {
    co_try$(Ml::Gguf::loadGguf("bundle://karm-ml/smollm2-360m-instruct-q8_0.gguf"_url));
    co_return Ok();
}
