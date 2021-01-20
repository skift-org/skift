#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Slider.h>
#include <libwidget/widgets/TitleBar.h>

#include "media-player/widgets/Cover.h"

namespace media_player
{

class Main : public Window
{
private:
public:
    Main() : Window(WINDOW_NONE | WINDOW_RESIZABLE)
    {
        icon(Icon::get("movie"));
        title("Media Player");
        size(Vec2i(700, 500));

        root()->layout(VFLOW(0));

        auto cover = new Cover(root(), Bitmap::load_from_or_placeholder("/Applications/media-player/cover.png"));

        cover->layout(VFLOW(0));
        cover->flags(Widget::FILL);

        new TitleBar(cover);
        new Spacer(cover);

        auto control_bar = new Container(cover);

        control_bar->insets(12);
        control_bar->layout(HFLOW(4));

        new Button(control_bar, Button::FILLED, Icon::get("play"));
        new Button(control_bar, Button::OUTLINE, Icon::get("stop"));
        new Button(control_bar, Button::OUTLINE, Icon::get("volume-high"));

        new Slider(control_bar);
    }
};

} // namespace media_player
