#pragma once

#include "node.h"

namespace Karm::Ui {

template <typename Crtp>
struct View : public Node<Crtp> {
    Math::Recti _bound;

    Math::Recti bound() const override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
    }
};

} // namespace Karm::Ui