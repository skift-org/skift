#include <libsystem/process/Process.h>
#include <libutils/FuzzyMatcher.h>
#include <libwidget/Button.h>
#include <libwidget/Label.h>
#include <libwidget/Window.h>

#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"

namespace panel
{

ApplicationListing::ApplicationListing(Widget *parent) : Container(parent)
{
    layout(VFLOW(4));
    flags(Widget::FILL);
    insets(Insetsi(4));

    render();
}

void ApplicationListing::filter(const String &filter)
{
    if (_filter == filter)
    {
        return;
    }

    _filter = filter;

    render();
}

void ApplicationListing::render()
{
    FuzzyMatcher matcher;

    clear_children();

    bool find_any = false;

    MenuEntry::load().foreach ([&](auto &entry) {
        if (!matcher.match(_filter, entry.name))
        {
            return Iteration::CONTINUE;
        }

        find_any = true;

        auto item = new Button(this, Button::TEXT, entry.icon, entry.name);

        item->insets(Insetsi(8));

        item->on(Event::ACTION, [this, entry](auto) {
            process_run(entry.command.cstring(), nullptr);
            window()->hide();
        });

        return Iteration::CONTINUE;
    });

    if (!find_any)
    {
        new Label(this, "No application found!", Anchor::CENTER);
    }
}

} // namespace panel
