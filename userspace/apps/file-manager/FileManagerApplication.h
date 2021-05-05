#pragma once

#include <libwidget/Application.h>

#include "file-manager/FileManagerWindow.h"

struct FileManagerApplication : public Widget::Application
{
    OwnPtr<Widget::Window> build()
    {
        return own<FileManagerWindow>();
    }
};