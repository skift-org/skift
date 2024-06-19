#pragma once

#include <karm-mime/uti.h>

#include "async.h"

namespace Karm::Sys {

Res<> launch(Mime::Uti const &itent, Mime::Url const &url);

Async::Task<> launchAsync(Mime::Uti const &itent, Mime::Url const &url);

} // namespace Karm::Sys
