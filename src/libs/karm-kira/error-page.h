#pragma once

#include <karm-ui/view.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child errorPageTitle(Mdi::Icon icon, String text);

Ui::Child errorPageSubTitle(String text);

Ui::Child errorPageBody(String text);

Ui::Child errorPageContent(Ui::Children children);

Ui::Child errorPageFooter(Ui::Children children);

Ui::Child errorPage(Mdi::Icon icon, String text, String body);

} // namespace Karm::Kira
