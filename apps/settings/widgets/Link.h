#pragma once

#include <libwidget/Button.h>

namespace Settings
{

class Link : public Button
{
public:
    Link(Widget *parent, RefPtr<Graphic::Icon> icon, String name);
};

} // namespace Settings
