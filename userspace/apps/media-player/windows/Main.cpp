#include <libgraphic/Icon.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Elements.h>
#include <libwidget/Slider.h>
#include <libwidget/TitleBar.h>

#include "media-player/windows/Main.h"

namespace MediaPlayer
{

Main::Main() : Window(WINDOW_NONE | WINDOW_RESIZABLE)
{
    size(Math::Vec2i(700, 500));

    root()->layout(VFLOW(0));

    auto cover = root()->add<Cover>(Graphic::Bitmap::load_from_or_placeholder("/Applications/media-player/cover.png"));

    cover->layout(VFLOW(0));
    cover->flags(Widget::Element::FILL);

    cover->add<Widget::TitleBar>(
        Graphic::Icon::get("movie"),
        "Media Player");

    cover->add(Widget::spacer());

    auto control_bar = cover->add<Widget::Container>();

    control_bar->insets(12);
    control_bar->layout(HFLOW(4));

    control_bar->add<Widget::Button>(Widget::Button::FILLED, Graphic::Icon::get("play"));
    control_bar->add<Widget::Button>(Widget::Button::OUTLINE, Graphic::Icon::get("stop"));

    control_bar->add(Widget::spacer());

    auto slider = control_bar->add<Widget::Slider>();
    slider->layout(HFLOW(0));
    slider->add<Widget::Button>(Widget::Button::TEXT, Graphic::Icon::get("volume-high"));
}

} // namespace MediaPlayer
