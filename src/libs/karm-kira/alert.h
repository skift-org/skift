#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child alertContent(Ui::Children children);

Ui::Child alertHeader(Ui::Children children);

Ui::Child alertTitle(String text);

Ui::Child alertDescription(String text);

Ui::Child alertFooter(Ui::Children children);

Ui::Child alertAction(Ui::OnPress onPress, String text);

Ui::Child alertCancel();

Ui::Child alert(String title, String description);

} // namespace Karm::Kira
