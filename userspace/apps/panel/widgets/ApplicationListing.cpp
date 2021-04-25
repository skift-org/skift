#include <libsystem/process/Process.h>
#include <libutils/FuzzyMatcher.h>
#include <libwidget/Button.h>
#include <libwidget/Label.h>
#include <libwidget/Window.h>

#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"

namespace panel
{

ApplicationListing::ApplicationListing() : VScroll()
{
    layout(VFLOW(4));
    flags(Element::FILL);

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

    host()->clear();
    host()->layout(VFLOW(4));

    bool find_any = false;

    MenuEntry::load().foreach ([&](auto &entry) {
        if (!matcher.match(_filter, entry.name))
        {
            return Iteration::CONTINUE;
        }

        find_any = true;

        auto item = host()->add<Widget::Button>(Widget::Button::TEXT, entry.image, entry.name);

        item->on(Widget::Event::ACTION, [this, entry](auto) {
            process_run(entry.command.cstring(), nullptr, 0);
            window()->hide();
        });

        return Iteration::CONTINUE;
    });

    if (!find_any)
    {
        host()->add<Widget::Label>("No application found!", Anchor::CENTER);
    }
}

} // namespace panel
