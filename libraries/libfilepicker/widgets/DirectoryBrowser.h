#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/Table.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class DirectoryBrowser : public Widget::Table
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<DirectoryListing> _listing;
    OwnPtr<Observer<Navigation>> _navigation_observer;

public:
    Optional<String> selected_path()
    {

        return process_resolve(_listing->info(selected()).name);
    }

    Callback<void(String &path)> on_element_selected;

    DirectoryBrowser(Widget::Component *parent, RefPtr<Navigation> navigation)
        : Widget::Table(parent), _navigation(navigation)
    {
        _listing = make<DirectoryListing>(navigation);
        model(_listing);

        flags(Component::FILL);

        empty_message("This directory is empty.");

        _navigation_observer = navigation->observe([this](auto &) {
            select(-1);
            scroll_to_top();
        });

        on(Widget::Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                if (_listing->info(selected()).type == FILE_TYPE_DIRECTORY)
                {
                    _navigation->navigate(_listing->info(selected()).name);
                }
                else if (on_element_selected)
                {
                    auto resolved_path = process_resolve(_listing->info(selected()).name);
                    on_element_selected(resolved_path);
                }
            }
        });
    }
};

} // namespace FilePicker