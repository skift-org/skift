#include <skift/Environment.h>

#include "panel/widgets/UserAccount.h"

namespace panel
{

UserAccount::UserAccount(Widget::Component *parent) : Widget::Label(parent, "")
{
    text(environment().get("POSIX").get("LOGNAME").as_string());
}

} // namespace panel
