#pragma once

#include <libwidget/Components.h>

namespace Panel
{

struct UserAccountComponent :
    public Widget::Component
{
    RefPtr<Widget::Element> build() override;
};

WIDGET_BUILDER(UserAccountComponent, user_account);

} // namespace Panel
