#pragma once

#include <libasync/Timer.h>
#include <libwidget/Components.h>
#include <libwidget/model/GraphModel.h>

namespace Panel
{

struct RessourceMonitor :
    public Widget::Component
{
private:
    Box<Async::Timer> _ram_timer;
    Ref<Widget::GraphModel> _ram_model;
    Box<Async::Timer> _cpu_timer;
    Ref<Widget::GraphModel> _cpu_model;

public:
    RessourceMonitor();

    Ref<Element> build() override;
};

WIDGET_BUILDER(RessourceMonitor, ressource_monitor);

} // namespace Panel
