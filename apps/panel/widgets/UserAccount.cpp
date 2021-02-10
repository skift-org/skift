#include <skift/Environment.h>

#include "panel/widgets/UserAccount.h"

namespace panel
{

UserAccount::UserAccount(Widget *parent) : Label(parent, "")
{
    text(environment().get("POSIX").get("LOGNAME").as_string());
}

} // namespace panel
