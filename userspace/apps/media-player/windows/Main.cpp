#include <libwidget/Components.h>
#include <libwidget/Elements.h>

#include "media-player/windows/Main.h"

namespace MediaPlayer
{

Main::Main() : Window(WINDOW_NONE | WINDOW_RESIZABLE)
{
    size(Math::Vec2i(700, 500));

    root()->DONT_USE_ME_layout(VFLOW(0));

    auto cover = root()->add<Cover>(Graphic::Bitmap::load_from_or_placeholder("/Applications/media-player/cover.png"));

    cover->DONT_USE_ME_layout(VFLOW(0));
    cover->flags(Widget::Element::FILL);

    cover->add(Widget::titlebar(Graphic::Icon::get("movie"), "Media Player"));

    cover->add(Widget::spacer());

    auto control_bar = cover->add<Widget::Element>();

    control_bar->insets(12);
    control_bar->DONT_USE_ME_layout(HFLOW(4));

    control_bar->add(Widget::filled_button(Graphic::Icon::get("play")));
    control_bar->add(Widget::outline_button(Graphic::Icon::get("stop")));

    control_bar->add(Widget::spacer());

    auto slider = control_bar->add(Widget::slider());
    slider->DONT_USE_ME_layout(HFLOW(0));
    slider->add(Widget::basic_button(Graphic::Icon::get("volume-high")));
}

} // namespace MediaPlayer
