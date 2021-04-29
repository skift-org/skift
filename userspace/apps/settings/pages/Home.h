#pragma once

#include <libwidget/Components.h>

namespace Settings
{

class HomePage : public Widget::Component
{
public:
    HomePage();

    RefPtr<Widget::Element> build() override;
};

} // namespace Settings
