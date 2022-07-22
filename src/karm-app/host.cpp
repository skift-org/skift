#include <embed/app.h>

#include "host.h"

namespace Karm::App {

Result<Strong<Karm::App::Host>> makeHost(Box<App::Client> &&client) {
    return Embed::makeHost(std::move(client));
}

} // namespace Karm::App
