#pragma once

#include <libwidget/Button.h>

namespace Settings
{

class Link : public Widget::Button
{
public:
    Link(RefPtr<Graphic::Icon> icon, String name);
};

} // namespace Settings
