#pragma once

#include <libwidget/Application.h>

#include "file-manager/FileManagerWindow.h"

struct FileManagerApplication : public Widget::Application
{
    Box<Widget::Window> build()
    {
        return own<FileManagerWindow>();
    }
};