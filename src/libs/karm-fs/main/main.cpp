#include <karm-sys/entry.h>
import Karm.Fs;
using namespace Karm;

Async::Task<> entryPointAsync(Sys::Context&) {
    auto file = co_trya$(Fs::Image::openOrCreateAsync("file:disk.raw"_url));
    co_trya$(file->truncateAsync(mib(512)));
    auto mbr = co_trya$(Fs::Mbr::formatAsync(file));

    auto root = co_trya$(Fs::createAsync<Fs::VDir>());
    auto volumes = co_trya$(root->createAsync("volumes"s, Sys::Type::DIR));
    co_trya$(volumes->linkAsync("sda", mbr));

    co_return Ok();
}