#include <libwidget/IconPanel.h>
#include <libwidget/TextField.h>

#include "panel/widgets/SearchBar.h"

namespace panel
{

SearchBar::SearchBar(RefPtr<Widget::TextModel> model) : Panel()
{
    layout(HFLOW(4));
    insets({6});
    outsets({8});
    border_radius(6);
    color(Widget::THEME_MIDDLEGROUND, Graphic::Colors::WHITE);
    layout(HFLOW(4));
    min_height(36);

    auto icon = add<Widget::IconPanel>(Graphic::Icon::get("search"));
    icon->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);

    auto field = add<Widget::TextField>(model);
    field->flags(Element::FILL);
    field->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);
    field->focus();
}

} // namespace panel