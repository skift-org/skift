#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Camera {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::CAMERA,
        .title = "Camera"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Camera
