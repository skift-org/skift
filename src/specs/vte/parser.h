#pragma once

import Karm.Core;   

namespace Vte {

struct Parser {
    enum struct State {
        NONE,

        GROUND,
        ANYWHERE,
        CSI_ENTRY,
        CSI_IGNORE,
        CSI_INTERMEDIATE,
        CSI_PARAM,
        DCS_ENTRY,
        DCS_IGNORE,
        DCS_INTERMEDIATE,
        DCS_PARAM,
        DCS_PASSTHROUGH,
        ESCAPE,
        ESCAPE_INTERMEDIATE,
        OSC_STRING,
        SOS_PM_APC_STRING,
        UTF8,
    };

    enum struct Action {
        NONE,

        PRINT,
        EXECUTE,
        HOOK,
        PUT,
        OSC_START,
        OSC_PUT,
        OSC_END,
        UNHOOK,
        CSI_DISPATCH,
        ESC_DISPATCH,
        ERROR,

        CLEAR,
        COLLECT,
        IGNORE,
        PARAM,
    };

    static constexpr usize ON_ENTRY = -1;
    static constexpr usize ON_EXIT = -2;

    State _state = State::GROUND;

    static constexpr usize MAX_CHARS = 16;
    Vec<u8> _chars;
    bool _hasIgnoreChars = false;

    static constexpr usize MAX_PARAMS = 16;
    Vec<usize> _params;
    bool _hasIgnoreParams = false;

    static Pair<Action, State> __transition(State s, usize c) {
#define STATE(FROM, START, END, ACTION, TO)           \
    if (s == State::FROM and START <= c and c <= END) \
        return {Action::ACTION, State::TO};
#include "defs/states.inc"
#undef STATE
        return {Action::NONE, State::NONE};
    }

    Pair<Action, State> _transition(State s, usize c) {
        auto [action, toState] = __transition(s, c);
        if (action != Action::NONE or toState != State::NONE)
            return {action, toState};
        return __transition(State::ANYWHERE, c);
    }

    auto _doAction(Action action, u8 b, auto& sink) {
        switch (action) {
        case Action::PRINT:
        case Action::EXECUTE:
        case Action::HOOK:
        case Action::PUT:
        case Action::OSC_START:
        case Action::OSC_PUT:
        case Action::OSC_END:
        case Action::UNHOOK:
        case Action::CSI_DISPATCH:
        case Action::ESC_DISPATCH:
            sink(action, b);
            break;

        case Action::IGNORE:
            break;

        case Action::COLLECT: {
            if (_chars.len() >= MAX_CHARS)
                _hasIgnoreChars = true;
            else
                _chars.pushBack(b);
            break;
        }

        case Action::PARAM: {
            if (b == ';') {
                if (_params.len() >= MAX_PARAMS)
                    _hasIgnoreParams = true;
                else
                    _params.pushBack(0);
            } else {
                if (_params.len() == 0)
                    _params.pushBack(0);

                if (_hasIgnoreParams)
                    break;

                auto& curr = last(_params);
                curr *= 10;
                curr += (b - '0');
            }
            break;
        }

        case Action::CLEAR:
            _chars.clear();
            _params.clear();
            _hasIgnoreChars = false;
            _hasIgnoreParams = false;
            break;

        default:
            sink(Action::ERROR, 0);
        }
    }

    Slice<u8> chars() const { return _chars; }

    Slice<usize> params() const { return _params; }

    void injest(u8 b, auto& sink) {
        auto [action, toState] = _transition(_state, b);

        if (toState != State::NONE) {
            auto [onEntry, _] = _transition(toState, ON_ENTRY);
            auto [onExit, _] = _transition(_state, ON_EXIT);

            if (onExit != Action::NONE)
                _doAction(onExit, 0, sink);

            if (action != Action::NONE)
                _doAction(action, b, sink);

            if (onEntry != Action::NONE)
                _doAction(onEntry, 0, sink);

            _state = toState;
        } else
            _doAction(action, b, sink);
    };
};

} // namespace Vte
