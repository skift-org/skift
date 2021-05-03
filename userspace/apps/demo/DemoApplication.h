#pragma once

#include <libwidget/Application.h>

#include "demo/DemoWindow.h"

struct DemoApplication : public Widget::Application
{
    OwnPtr<Widget::Window> build() override
    {
        return own<DemoWindow>();
    }
};
