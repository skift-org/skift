#pragma once

#include <karm-io/emit.h>

namespace Karm::Cli {

struct Cmd {
    enum Type {
        SAVE,
        RESTORE,

        UP,
        DOWN,
        FORWARD,
        BACKWARD,
        NEXTLINE,
        PREVLINE,
        HORIZONTAL,
        POSITION,
        HIDE,
        SHOW,

        CLEAR_DISPLAY,
        CLEAR_DISPLAY_AFTER,
        CLEAR_DISPLAY_BEFORE,
        CLEAR_LINE,
        CLEAR_LINE_AFTER,
        CLEAR_LINE_BEFORE,
    };

    Type _type;
    int _row = 0;
    int _col = 0;

    static Cmd save() {
        return {SAVE};
    }

    static Cmd restore() {
        return {RESTORE};
    }

    static Cmd up(int count) {
        return {UP, count};
    }

    static Cmd down(int count) {
        return {DOWN, count};
    }

    static Cmd forward(int count) {
        return {FORWARD, 0, count};
    }

    static Cmd backward(int count) {
        return {BACKWARD, 0, count};
    }

    static Cmd nextline(int count) {
        return {NEXTLINE, count};
    }

    static Cmd prevline(int count) {
        return {PREVLINE, count};
    }

    static Cmd horizontal(int count) {
        return {HORIZONTAL, 0, count};
    }

    static Cmd position(int row, int col) {
        return {POSITION, row, col};
    }

    static Cmd hide() {
        return {HIDE};
    }

    static Cmd show() {
        return {SHOW};
    }

    static Cmd clearDisplay() {
        return {CLEAR_DISPLAY};
    }

    static Cmd clearDisplayAfter() {
        return {CLEAR_DISPLAY_AFTER};
    }

    static Cmd clearDisplayBefore() {
        return {CLEAR_DISPLAY_BEFORE};
    }

    static Cmd clearLine() {
        return {CLEAR_LINE};
    }

    static Cmd clearLineAfter() {
        return {CLEAR_LINE_AFTER};
    }

    static Cmd clearLineBefore() {
        return {CLEAR_LINE_BEFORE};
    }

    void repr([[maybe_unused]] Io::Emit &e) const {
#ifdef __ck_karm_cli_backend_ansi__
        switch (_type) {
        case Cli::Cmd::SAVE:
            e("\x1b[s");
            break;

        case Cli::Cmd::RESTORE:
            e("\x1b[u");
            break;

        case Cli::Cmd::UP:
            e("\x1b[{}A", _row);
            break;

        case Cli::Cmd::DOWN:
            e("\x1b[{}B", _row);
            break;

        case Cli::Cmd::FORWARD:
            e("\x1b[{}C", _col);
            break;

        case Cli::Cmd::BACKWARD:
            e("\x1b[{}D", _col);
            break;

        case Cli::Cmd::NEXTLINE:
            e("\x1b[{}E", _row);
            break;

        case Cli::Cmd::PREVLINE:
            e("\x1b[{}F", _row);
            break;

        case Cli::Cmd::HORIZONTAL:
            e("\x1b[{}G", _col);
            break;

        case Cli::Cmd::POSITION:
            e("\x1b[{};{}H", _row, _col);
            break;

        case Cli::Cmd::HIDE:
            e("\x1b[?25l");
            break;

        case Cli::Cmd::SHOW:
            e("\x1b[?25h");
            break;

        case Cli::Cmd::CLEAR_DISPLAY:
            e("\x1b[2J");
            break;

        case Cli::Cmd::CLEAR_DISPLAY_AFTER:
            e("\x1b[J");
            break;

        case Cli::Cmd::CLEAR_DISPLAY_BEFORE:
            e("\x1b[1J");
            break;

        case Cli::Cmd::CLEAR_LINE:
            e("\x1b[2K");
            break;

        case Cli::Cmd::CLEAR_LINE_AFTER:
            e("\x1b[K");
            break;

        case Cli::Cmd::CLEAR_LINE_BEFORE:
            e("\x1b[1K");
            break;

        default:
            panic("Invalid cursor move type");
        }
#endif
    }
};

} // namespace Karm::Cli
