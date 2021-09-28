#pragma once

#include <libsystem/process/Launchpad.h>

#include <libfilepicker/model/FilesystemModel.h>
#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>

namespace FilePicker
{

struct FileBrowser : public Browser
{
public:
    Func<void(String &path)> on_element_selected;

    FileBrowser(RefPtr<Navigation> navigation)
        : Browser(navigation)
    {
        _listing = make<FilesystemModel>(navigation);
        model(_listing);

        on(Widget::Event::ACTION, [this](auto) {
            if (selected() >= 0)
            {
                if (_listing->info(selected()).type == HJ_FILE_TYPE_DIRECTORY)
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