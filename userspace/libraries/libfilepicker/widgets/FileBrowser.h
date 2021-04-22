#pragma once

#include <libsystem/process/Launchpad.h>

#include <libfilepicker/model/FileListing.h>
#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>

namespace FilePicker
{

class FileBrowser : public Browser
{
private:
    RefPtr<FileListing> _listing;

public:
    Optional<String> selected_path() override
    {
        if (selected() == -1)
        {
            return {};
        }
        return process_resolve(_listing->info(selected()).name);
    }

    Callback<void(String &path)> on_element_selected;

    FileBrowser(Widget::Component *parent, RefPtr<Navigation> navigation)
        : Browser(parent, navigation)
    {
        _listing = make<FileListing>(navigation);
        model(_listing);

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