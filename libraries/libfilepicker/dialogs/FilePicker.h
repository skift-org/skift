#pragma once

#include <libwidget/dialog/Dialog.h>

#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class Dialog : public ::Dialog
{
private:
    RefPtr<Navigation> _navigation = nullptr;
    Optional<String> _selected_file{};

public:
    Dialog();

    ~Dialog();

    Optional<String> selected_file() const { return _selected_file; }

    virtual void render(Window *window);
};

} // namespace filepicker