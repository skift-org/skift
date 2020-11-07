#pragma once

#include <abi/Syscalls.h>

#include <libsystem/eventloop/Timer.h>
#include <libutils/StringBuilder.h>
#include <libwidget/Widgets.h>

#include "task-manager/TaskModel.h"

class RAMGraph : public Graph
{
private:
    RefPtr<TaskModel> _model;

    Label *_label_usage;
    Label *_label_available;
    Label *_label_greedy;

    OwnPtr<Timer> _graph_timer{};
    OwnPtr<Timer> _text_timer{};

public:
    RAMGraph(Widget *parent, RefPtr<TaskModel> model)
        : Graph(parent, 256, Colors::ROYALBLUE),
          _model(model)
    {
        layout(VFLOW(0));
        insets(Insets(8));
        attributes(LAYOUT_FILL);

        auto icon_and_text = new Container(this);
        icon_and_text->layout(HFLOW(4));
        new IconPanel(icon_and_text, Icon::get("chip"));
        new Label(icon_and_text, "Memory");

        auto cpu_filler = new Container(this);
        cpu_filler->attributes(LAYOUT_FILL);

        _label_usage = new Label(this, "Usage: nil Mio", Position::RIGHT);
        _label_available = new Label(this, "Available: nil Mio", Position::RIGHT);
        _label_greedy = new Label(this, "Most greedy: nil", Position::RIGHT);

        _graph_timer = own<Timer>(500, [&]() {
            SystemStatus status{};
            hj_system_status(&status);

            record(status.used_ram / (float)status.total_ram);
        });

        _graph_timer->start();

        _text_timer = own<Timer>(1000, [&]() {
            SystemStatus status{};
            hj_system_status(&status);

            unsigned usage = status.used_ram / 1024 / 1024;
            char buffer_usage[50];
            snprintf(buffer_usage, 50, "Usage: %u Mio", usage);
            _label_usage->text(buffer_usage);

            unsigned avaliable = status.total_ram / 1024 / 1024;
            char buffer_avaliable[50];
            snprintf(buffer_avaliable, 50, "Avaliable: %u Mio", avaliable);
            _label_available->text(buffer_avaliable);

            auto greedy = _model->ram_greedy();
            _label_greedy->text(StringBuilder().append("Most greedy: ").append(greedy).finalize());
        });

        _text_timer->start();
    }
};
