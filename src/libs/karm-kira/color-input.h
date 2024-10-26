#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child hsvSquare(Gfx::Hsv value, Ui::OnChange<Gfx::Hsv> onChange);

Ui::Child hsvValueSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange);

Ui::Child hsvSaturationSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange);

Ui::Child hsvHueSlider(Gfx::Hsv hsv, Ui::OnChange<Gfx::Hsv> onChange);

Ui::Child colorInput(Gfx::Color color, Ui::OnChange<Gfx::Color> onChange);

} // namespace Karm::Kira
