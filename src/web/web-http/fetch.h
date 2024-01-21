#pragma once

#include <json/json.h>
#include <karm-sys/async.h>
#include <url/url.h>

namespace Web::Client {

Async::Task<usize> fetch(Url::Url const &url, Io::Writer &out);

Async::Task<String> fetchString(Url::Url const &url);

Async::Task<Json::Value> fetchJson(Url::Url const &url);

} // namespace Web::Client
