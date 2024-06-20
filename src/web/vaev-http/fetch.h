#pragma once

#include <karm-mime/url.h>
#include <karm-sys/async.h>
#include <vaev-json/json.h>

namespace Vaev::Client {

Async::Task<usize> fetch(Mime::Url const &url, Io::Writer &out);

Async::Task<String> fetchString(Mime::Url const &url);

Async::Task<Json::Value> fetchJson(Mime::Url const &url);

} // namespace Vaev::Client
