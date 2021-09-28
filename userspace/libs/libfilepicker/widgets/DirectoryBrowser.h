#pragma once

#include <libsystem/process/Launchpad.h>

#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/Browser.h>

namespace FilePicker
{

struct DirectoryBrowser : public Browser
{
public:
    DirectoryBrowser(RefPtr<Navigation> navigation)
        : Browser(navigation)
    {
        _listing = make<FilesystemModel>(navigation, [](IO::Directory::Entry &entry) {
            return entry.stat.type == HJ_FILE_TYPE_DIRECTORY;
        });
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