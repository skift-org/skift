#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/widgets/Table.h>

#include <libfilepicker/model/Listing.h>
#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class Browser : public Table
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Listing> _listing;

public:
    Browser(Widget *parent, RefPtr<Navigation> navigation)
        : Table(parent), _navigation(navigation)
    {
        _listing = make<Listing>(navigation);
        model(_listing);

        flags(Widget::FILL);

        empty_message("This directory is empty.");

        on(Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                if (_listing->info(selected()).type == FILE_TYPE_DIRECTORY)
                {
                    _navigation->navigate(_listing->info(selected()).name);
                }
                else
                {
                    auto l = launchpad_create("open", "/System/Binaries/open");
                    launchpad_argument(l, _listing->info(selected()).name.cstring());
                    launchpad_launch(l, nullptr);
                }
            }
        });
    }
};

} // namespace filepicker