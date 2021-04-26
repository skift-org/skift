#pragma once

#include <libwidget/Component.h>

namespace panel
{

class UserAccount : public Widget::Component
{
public:
    UserAccount();

    RefPtr<Widget::Element> build() override;
};

} // namespace panel
