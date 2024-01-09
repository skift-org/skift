#pragma once

#include <json/json.h>
#include <karm-sys/async.h>
#include <url/url.h>

namespace Web::Client {

Sys::Task<usize> fetch(Url::Url const &url, Io::Writer &out);

Sys::Task<String> fetchString(Url::Url const &url);

Sys::Task<Json::Value> fetchJson(Url::Url const &url);

} // namespace Web::Client
