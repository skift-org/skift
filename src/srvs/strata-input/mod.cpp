export module Strata.Input;

import Karm.Core;
import Karm.App;
import Karm.Logger;

using namespace Karm;

namespace Strata::Input {

export struct Keymap {
    struct Action {
        Flags<App::KeyMod> mods;
        String out;
    };

    Map<App::Key, Vec<Action>> _mappings;

    void bind(App::Key key, Str out, Flags<App::KeyMod> mods = {}) {
        auto& actions = _mappings.getOrDefault(key);
        actions.pushBack({mods, out});
    }

    void bind(App::Key key, Str lower, Str upper, Str alt = ""s) {
        bind(key, upper, App::KeyMod::SHIFT);
        bind(key, lower);
        if (alt)
            bind(key, alt, App::KeyMod::ALT);
    }

    static Keymap qwerty() {
        Keymap map;

        // --- Row 1 (Numbers) ---
        map.bind(App::Key::NUM1, "1", "!", "¡");
        map.bind(App::Key::NUM2, "2", "@", "™");
        map.bind(App::Key::NUM3, "3", "#", "£");
        map.bind(App::Key::NUM4, "4", "$", "¢");
        map.bind(App::Key::NUM5, "5", "%", "∞");
        map.bind(App::Key::NUM6, "6", "^", "§");
        map.bind(App::Key::NUM7, "7", "&", "¶");
        map.bind(App::Key::NUM8, "8", "*", "•");
        map.bind(App::Key::NUM9, "9", "(", "ª");
        map.bind(App::Key::NUM0, "0", ")", "º");
        map.bind(App::Key::SYM1, "-", "_", "–"); // Minus
        map.bind(App::Key::SYM2, "=", "+", "≠"); // Equal

        // --- Row 2 (QWERTY) ---
        map.bind(App::Key::Q, "q", "Q", "œ");
        map.bind(App::Key::W, "w", "W", "∑");
        map.bind(App::Key::E, "e", "E", "´");
        map.bind(App::Key::R, "r", "R", "®");
        map.bind(App::Key::T, "t", "T", "†");
        map.bind(App::Key::Y, "y", "Y", "¥");
        map.bind(App::Key::U, "u", "U", "¨");
        map.bind(App::Key::I, "i", "I", "ˆ");
        map.bind(App::Key::O, "o", "O", "ø");
        map.bind(App::Key::P, "p", "P", "π");
        map.bind(App::Key::SYM3, "[", "{", "“"); // Left Bracket
        map.bind(App::Key::SYM4, "]", "}", "‘"); // Right Bracket

        // --- Row 3 (Home Row) ---
        map.bind(App::Key::A, "a", "A", "å");
        map.bind(App::Key::S, "s", "S", "ß");
        map.bind(App::Key::D, "d", "D", "∂");
        map.bind(App::Key::F, "f", "F", "ƒ");
        map.bind(App::Key::G, "g", "G", "©");
        map.bind(App::Key::H, "h", "H", "˙");
        map.bind(App::Key::J, "j", "J", "∆");
        map.bind(App::Key::K, "k", "K", "˚");
        map.bind(App::Key::L, "l", "L", "¬");
        map.bind(App::Key::SYM5, ";", ":", "…");  // Semicolon
        map.bind(App::Key::SYM6, "'", "\"", "æ"); // Quote
        map.bind(App::Key::SYM7, "`", "~", "`");  // Backtick
        map.bind(App::Key::SYM8, "\\", "|", "«"); // Backslash

        // --- Row 4 (Bottom) ---
        map.bind(App::Key::Z, "z", "Z", "Ω");
        map.bind(App::Key::X, "x", "X", "≈");
        map.bind(App::Key::C, "c", "C", "ç");
        map.bind(App::Key::V, "v", "V", "√");
        map.bind(App::Key::B, "b", "B", "∫");
        map.bind(App::Key::N, "n", "N", "˜");
        map.bind(App::Key::M, "m", "M", "µ");
        map.bind(App::Key::SYM9, ",", "<", "≤");  // Comma
        map.bind(App::Key::SYM10, ".", ">", "≥"); // Period
        map.bind(App::Key::SYM11, "/", "?", "÷"); // Slash

        // --- Special Keys ---
        map.bind(App::Key::SPACE, " ", " ");
        map.bind(App::Key::ENTER, "\n", "\n");
        map.bind(App::Key::KPADENTER, "\n", "\n");
        map.bind(App::Key::TAB, "\t", "\t");

        // --- Keypad ---
        map.bind(App::Key::KPAD0, "0", "0");
        map.bind(App::Key::KPAD1, "1", "1");
        map.bind(App::Key::KPAD2, "2", "2");
        map.bind(App::Key::KPAD3, "3", "3");
        map.bind(App::Key::KPAD4, "4", "4");
        map.bind(App::Key::KPAD5, "5", "5");
        map.bind(App::Key::KPAD6, "6", "6");
        map.bind(App::Key::KPAD7, "7", "7");
        map.bind(App::Key::KPAD8, "8", "8");
        map.bind(App::Key::KPAD9, "9", "9");
        map.bind(App::Key::SYM13, "-", "-"); // Kpad Minus
        map.bind(App::Key::SYM14, "+", "+"); // Kpad Plus
        map.bind(App::Key::SYM15, ".", "."); // Kpad Dot
        map.bind(App::Key::SYM16, "*", "*"); // Kpad Star

        return map;
    }

    Opt<Str> handle(App::Key key, Flags<App::KeyMod> mods) {
        if (not _mappings.has(key))
            return NONE;

        auto& actions = _mappings.get(key);
        for (auto& action : actions)
            if (App::match(mods, action.mods))
                return action.out;
        return NONE;
    }
};

} // namespace Strata::Input