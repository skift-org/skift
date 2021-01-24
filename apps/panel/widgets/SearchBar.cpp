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
    color(THEME_MIDDLEGROUND, Colors::WHITE);
    layout(HFLOW(4));

    auto icon = new IconPanel(this, Icon::get("search"));
    icon->color(THEME_FOREGROUND, Colors::BLACK);

    auto field = new TextField(this, model);
    field->flags(Widget::FILL);
    field->color(THEME_FOREGROUND, Colors::BLACK);
    field->focus();
}

} // namespace panel