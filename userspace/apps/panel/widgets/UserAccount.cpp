#include <skift/Environment.h>

#include "panel/widgets/UserAccount.h"

namespace panel
{

UserAccount::UserAccount()
    : Widget::Label(environment().get("POSIX").get("LOGNAME").as_string())
{
}

} // namespace panel
