#include <libwidget/IconPanel.h>
#include <libwidget/TextField.h>

#include "panel/widgets/SearchBar.h"

namespace panel
{

SearchBar::SearchBar(Widget *parent, RefPtr<TextModel> model) : Panel(parent)
{
    layout(HFLOW(4));
    insets({6});
    outsets({8});
    border_radius(6);
    color(THEME_MIDDLEGROUND, Graphic::Colors::WHITE);
    layout(HFLOW(4));

    auto icon = new IconPanel(this, Graphic::Icon::get("search"));
    icon->color(THEME_FOREGROUND, Graphic::Colors::BLACK);

    auto field = new TextField(this, model);
    field->flags(Widget::FILL);
    field->color(THEME_FOREGROUND, Graphic::Colors::BLACK);
    field->focus();
}

} // namespace panel