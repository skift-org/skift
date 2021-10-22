#pragma once

#include <libwidget/Application.h>

#include "terminal/TerminalWindow.h"

struct TerminalApplication :
    public Widget::Application
{
    Box<Widget::Window> build()
    {
        return own<TerminalWindow>();
    }
};