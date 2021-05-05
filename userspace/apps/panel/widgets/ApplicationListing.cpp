#include <libsystem/process/Process.h>
#include <libutils/FuzzyMatcher.h>
#include <libwidget/Elements.h>
#include <libwidget/Window.h>

#include "libutils/Vector.h"
#include "libwidget/Element.h"
#include "libwidget/elements/ScrollElement.h"
#include "libwidget/layouts/FlowLayout.h"
#include "panel/model/MenuEntry.h"
#include "panel/widgets/ApplicationListing.h"

namespace panel
{

ApplicationListing::ApplicationListing(String filter)
    : _filter{filter}
{
    flags(Element::FILL);
}

RefPtr<Widget::Element> ApplicationListing::build()
{
    FuzzyMatcher matcher;

    Vector<RefPtr<Widget::Element>> childs;

    MenuEntry::load().foreach ([&](auto &entry) {
        if (!matcher.match(_filter, entry.name))
        {
            return Iteration::CONTINUE;
        }

        childs.push_back(Widget::basic_button(entry.image, entry.name, [this, entry] {
            process_run(entry.command.cstring(), nullptr, 0);
            window()->hide();
        }));

        return Iteration::CONTINUE;
    });

    if (childs.count() == 0)
    {
        childs.push_back(Widget::label("No application found!", Anchor::CENTER));
    }

    return Widget::scroll(Widget::vflow(4, childs));
}

} // namespace panel
