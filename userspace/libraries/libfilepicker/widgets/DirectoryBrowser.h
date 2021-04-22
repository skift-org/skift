#pragma once

#include <libsystem/process/Launchpad.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>

namespace FilePicker
{

class DirectoryBrowser : public Browser
{
private:
    RefPtr<DirectoryListing> _listing;

public:
    Optional<String> selected_path() override
    {
        if (selected() == -1)
        {
            return {};
        }
        return process_resolve(_listing->info(selected()).name);
    }

    DirectoryBrowser(Widget::Component *parent, RefPtr<Navigation> navigation)
        : Browser(parent, navigation)
    {
        _listing = make<DirectoryListing>(navigation);
        model(_listing);

        on(Widget::Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                _navigation->navigate(_listing->info(selected()).name);
            }
        });
    }
};

} // namespace FilePicker