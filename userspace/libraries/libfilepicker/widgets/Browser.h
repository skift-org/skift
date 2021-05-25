#pragma once

#include <libsystem/process/Launchpad.h>

#include <libwidget/Views.h>

#include <libfilepicker/model/FilesystemModel.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

struct Browser : public Widget::Table
{
protected:
    RefPtr<Navigation> _navigation;
    OwnPtr<Async::Observer<Navigation>> _navigation_observer;
    RefPtr<FilesystemModel> _listing;

public:
    Optional<String> selected_path()
    {
        if (selected() == -1)
        {
            return NONE;
        }

        return process_resolve(_listing->info(selected()).name);
    }

    Browser(RefPtr<Navigation> navigation)
        : Widget::Table(), _navigation(navigation)
    {
        flags(Element::FILL);

        empty_message("This directory is empty.");

        _navigation_observer = navigation->observe([this](auto &) {
            select(-1);
            scroll_to_top();
        });
    }
};

} // namespace FilePicker