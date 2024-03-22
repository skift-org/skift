#pragma once

#include <karm-mime/url.h>
#include <karm-sys/async.h>
#include <web-json/json.h>

namespace Web::Client {

Async::Task<usize> fetch(Mime::Url const &url, Io::Writer &out);

Async::Task<String> fetchString(Mime::Url const &url);

Async::Task<Json::Value> fetchJson(Mime::Url const &url);

} // namespace Web::Client
