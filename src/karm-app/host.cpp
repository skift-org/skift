#include <embed/app.h>

#include "host.h"

namespace Karm::App {

Result<Strong<Karm::App::Host>> makeHost() {
    return Embed::makeHost();
}

} // namespace Karm::App
