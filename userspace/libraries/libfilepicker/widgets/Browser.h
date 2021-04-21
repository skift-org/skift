#pragma once

#include <libsystem/process/Launchpad.h>
#include <libwidget/Table.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class Browser : public Widget::Table
{
protected:
    RefPtr<Navigation> _navigation;
    OwnPtr<Async::Observer<Navigation>> _navigation_observer;

public:
    virtual Optional<String> selected_path() = 0;

    Browser(Widget::Component *parent, RefPtr<Navigation> navigation)
        : Widget::Table(parent), _navigation(navigation)
    {
        flags(Component::FILL);

        empty_message("This directory is empty.");

        _navigation_observer = navigation->observe([this](auto &) {
            select(-1);
            scroll_to_top();
        });
    }
};

} // namespace FilePicker