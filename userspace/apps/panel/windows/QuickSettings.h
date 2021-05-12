#pragma once

#include <libwidget/Window.h>

namespace Panel
{

struct QuickSettings : public Widget::Window
{
    QuickSettings();
    RefPtr<Widget::Element> build() override;
};

} // namespace Panel
