#include <libwidget/IconPanel.h>
#include <libwidget/TextField.h>

#include "panel/widgets/SearchBar.h"

namespace panel
{

SearchBar::SearchBar(Widget::Component *parent, RefPtr<Widget::TextModel> model) : Panel(parent)
{
    layout(HFLOW(4));
    insets({6});
    outsets({8});
    border_radius(6);
    color(Widget::THEME_MIDDLEGROUND, Graphic::Colors::WHITE);
    layout(HFLOW(4));

    auto icon = new Widget::IconPanel(this, Graphic::Icon::get("search"));
    icon->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);

    auto field = new Widget::TextField(this, model);
    field->flags(Component::FILL);
    field->color(Widget::THEME_FOREGROUND, Graphic::Colors::BLACK);
    field->focus();
}

} // namespace panel