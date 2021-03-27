#include <libgraphic/Icon.h>
#include <libwidget/Button.h>
#include <libwidget/Container.h>
#include <libwidget/Slider.h>
#include <libwidget/Spacer.h>
#include <libwidget/TitleBar.h>

#include "media-player/windows/Main.h"

namespace MediaPlayer
{

Main::Main() : Window(WINDOW_NONE | WINDOW_RESIZABLE)
{
    icon(Graphic::Icon::get("movie"));
    title("Media Player");
    size(Vec2i(700, 500));

    root()->layout(VFLOW(0));

    auto cover = new Cover(root(), Graphic::Bitmap::load_from_or_placeholder("/Applications/media-player/cover.png"));

    cover->layout(VFLOW(0));
    cover->flags(Widget::Component::FILL);

    new Widget::TitleBar(cover);
    new Widget::Spacer(cover);

    auto control_bar = new Widget::Container(cover);

    control_bar->insets(12);
    control_bar->layout(HFLOW(4));

    new Widget::Button(control_bar, Widget::Button::FILLED, Graphic::Icon::get("play"));
    new Widget::Button(control_bar, Widget::Button::OUTLINE, Graphic::Icon::get("stop"));
    new Widget::Button(control_bar, Widget::Button::OUTLINE, Graphic::Icon::get("volume-high"));

    new Widget::Slider(control_bar);
}

} // namespace MediaPlayer
