#pragma once

#include <libgraphic/Icon.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Slider.h>

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
        cover->attributes(LAYOUT_FILL);

        auto control_bar = new Panel(root());

        control_bar->layout(HFLOW(4));
        control_bar->insets({6, 4});

        new Button(control_bar, ButtonStyle::BUTTON_FILLED, Icon::get("play"));
        new Button(control_bar, ButtonStyle::BUTTON_OUTLINE, Icon::get("stop"));
        new Button(control_bar, ButtonStyle::BUTTON_OUTLINE, Icon::get("volume-high"));

        new Slider(control_bar);
    }
};

} // namespace media_player
