#pragma once

#include <libwidget/Button.h>

namespace Settings
{

class Link : public Button
{
private:
public:
    Link(Widget *parent, RefPtr<Icon> icon, String name);
};

} // namespace Settings
