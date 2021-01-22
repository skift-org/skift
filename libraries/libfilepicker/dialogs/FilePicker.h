#pragma once

#include <libwidget/dialog/Dialog.h>

#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class Dialog : public ::Dialog
{
private:
    RefPtr<Navigation> _navigation = nullptr;

public:
    Dialog();

    ~Dialog();

    virtual void render(Window *window);
};

} // namespace filepicker