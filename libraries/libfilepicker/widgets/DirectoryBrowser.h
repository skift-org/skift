#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/Table.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class DirectoryBrowser : public Table
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<DirectoryListing> _listing;
    OwnPtr<Observer<Navigation>> _navigation_observer;

public:
    DirectoryBrowser(Widget *parent, RefPtr<Navigation> navigation)
        : Table(parent), _navigation(navigation)
    {
        _listing = make<DirectoryListing>(navigation);
        model(_listing);

        flags(Widget::FILL);

        empty_message("This directory is empty.");

        _navigation_observer = navigation->observe([this](auto &) {
            select(-1);
            scroll_to_top();
        });

        on(Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                if (_listing->info(selected()).type == FILE_TYPE_DIRECTORY)
                {
                    _navigation->navigate(_listing->info(selected()).name);
                }
                else
                {
                    auto l = launchpad_create("open", "/System/Utilities/open");
                    launchpad_argument(l, _listing->info(selected()).name.cstring());
                    launchpad_launch(l, nullptr);
                }
            }
        });
    }
};

} // namespace filepicker