#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child dialogContent(Ui::Children children);

Ui::Child dialogTitleBar(String title);

Ui::Child dialogHeader(Ui::Children children);

Ui::Child dialogTitle(String text);

Ui::Child dialogDescription(String text);

Ui::Child dialogFooter(Ui::Children children);

Ui::Child dialogAction(Ui::OnPress onPress, String text);

Ui::Child dialogCancel();

Ui::Child alert(String title, String description);

} // namespace Karm::Kira
