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
    OwnPtr<Async::Timer> _ram_timer;
    RefPtr<Widget::GraphModel> _ram_model;
    OwnPtr<Async::Timer> _cpu_timer;
    RefPtr<Widget::GraphModel> _cpu_model;

public:
    RessourceMonitor();

    RefPtr<Element> build() override;
};

WIDGET_BUILDER(RessourceMonitor, ressource_monitor);

} // namespace Panel
