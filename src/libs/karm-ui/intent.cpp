module;

#include <karm-app/inputs.h>
#include <karm-base/func.h>

export module Karm.Ui:intent;

import :node;
import :funcs;

namespace Karm::Ui {

export struct KeyboardShortcutActivated {};

struct Intent : ProxyNode<Intent> {
    Send<App::Event&> _map;

    Intent(Send<App::Event&> map, Child child)
        : ProxyNode<Intent>(std::move(child)), _map(std::move(map)) {}

    void reconcile(Intent& o) override {
        _map = std::move(o._map);
        ProxyNode<Intent>::reconcile(o);
    }

    void event(App::Event& e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::event(e);
    }

    void bubble(App::Event& e) override {
        if (e.accepted())
            return;
        _map(*this, e);
        ProxyNode<Intent>::bubble(e);
    }
};

export Child intent(Send<App::Event&> filter, Child child) {
    return makeRc<Intent>(filter, std::move(child));
}

export auto intent(Send<App::Event&> filter) {
    return [filter](Child child) mutable {
        return intent(filter, std::move(child));
    };
}

export auto keyboardShortcut(App::Key key, App::KeyMod mods, Send<> onPress) {
    return intent([=](Ui::Node& n, App::Event& e) {
        if (auto it = e.is<App::KeyboardEvent>();
            it and it->type == App::KeyboardEvent::PRESS and it->key == key and match(it->mods, mods)) {
            onPress(n);
            e.accept();
        }
    });
}

export auto keyboardShortcut(App::Key key, Send<> onPress) {
    return keyboardShortcut(
        key, App::KeyMod::NONE, onPress
    );
}

export auto keyboardShortcut(App::Key key, App::KeyMod mods = App::KeyMod::NONE) {
    return keyboardShortcut(
        key, mods, [=](Ui::Node& n) {
            event<KeyboardShortcutActivated>(n);
        }
    );
}

} // namespace Karm::Ui
