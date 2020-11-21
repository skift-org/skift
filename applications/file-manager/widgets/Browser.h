#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/widgets/Table.h>

#include "file-manager/model/Listing.h"
#include "file-manager/model/Navigation.h"

namespace file_manager
{

class Browser : public Table
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Listing> _listing;

public:
    Browser(Widget *parent, RefPtr<Navigation> navigation)
        : Table(parent),
          _navigation(navigation)
    {
        _listing = make<Listing>(navigation);
        model(_listing);

        attributes(LAYOUT_FILL);
        empty_message("This directory is empty.");

        on(Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                if (_listing->file_type(selected()) == FILE_TYPE_DIRECTORY)
                {
                    _navigation->navigate(_listing->file_name(selected()));
                }
                else
                {
                    auto l = launchpad_create("open", "/System/Binaries/open");
                    launchpad_argument(l, strdup(_listing->file_name(selected()).cstring()));
                    launchpad_launch(l, nullptr);
                }
            }
        });
    }

    ~Browser() {}
};

} // namespace file_manager
