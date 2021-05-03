#pragma once

#include <libwidget/Components.h>

namespace panel
{

class UserAccount : public Widget::Component
{
public:
    UserAccount();

    RefPtr<Widget::Element> build() override;
};

WIDGET_BUILDER(UserAccount, user_account);

} // namespace panel
