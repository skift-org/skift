#pragma once

#include <karm-mime/url.h>
#include <karm-net/json/json.h>
#include <karm-sys/async.h>

namespace Karm::Net::Http {

Async::Task<usize> fetch(Mime::Url const &url, Io::Writer &out);

Async::Task<String> fetchString(Mime::Url const &url);

Async::Task<Json::Value> fetchJson(Mime::Url const &url);

} // namespace Karm::Net::Http
