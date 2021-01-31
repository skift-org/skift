#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/Table.h>

#include <libfilepicker/model/ArchiveListing.h>
#include <libfilepicker/model/Navigation.h>

#include <libfile/Archive.h>

namespace filepicker
{

class ArchiveBrowser : public Table
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Archive> _archive;
    RefPtr<ArchiveListing> _listing;
    OwnPtr<Observer<Navigation>> _navigation_observer;

public:
    ArchiveBrowser(Widget *parent, RefPtr<Navigation> navigation, RefPtr<Archive> archive)
        : Table(parent), _navigation(navigation), _archive(archive)
    {
        _listing = make<ArchiveListing>(navigation, archive);
        model(_listing);

        flags(Widget::FILL);

        empty_message("This archive is empty.");

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
                    // FIXME: Extract and Open the file.
                }
            }
        });
    }
};

} // namespace filepicker