#pragma once

#include <json/json.h>
#include <karm-async/async.h>
#include <url/url.h>

namespace Web::Client {

Async::Prom<usize> fetch(Url::Url const &url, Io::Writer &out);

Async::Prom<String> fetchString(Url::Url const &url);

Async::Prom<Json::Value> fetchJson(Url::Url const &url);

} // namespace Web::Client
