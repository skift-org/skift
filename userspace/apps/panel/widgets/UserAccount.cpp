#include <skift/Environment.h>

#include <libwidget/Elements.h>

#include "panel/widgets/UserAccount.h"

namespace panel
{

UserAccount::UserAccount() {}

RefPtr<Widget::Element> UserAccount::build()
{
    return Widget::label(environment().get("POSIX").get("LOGNAME").as_string());
}

} // namespace panel
