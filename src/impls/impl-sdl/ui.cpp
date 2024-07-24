#include <SDL.h>
#include <karm-ui/drag.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

static SDL_HitTestResult _hitTestCallback(SDL_Window *window, SDL_Point const *area, void *data);

struct SdlHost :
    public Host {
    SDL_Window *_window{};

    Math::Vec2i _lastMousePos{};
    Math::Vec2i _lastScreenMousePos{};

    SdlHost(Child root, SDL_Window *window)
        : Host(root), _window(window) {
    }

    ~SdlHost() {
        SDL_DestroyWindow(_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Quit();
    }

    Math::Recti bound() override {
        i32 w, h;
        SDL_GetWindowSize(_window, &w, &h);
        return {w, h};
    }

    f64 dpi() {
        return pixels().width() / (f64)bound().width;
    }

    Gfx::MutPixels mutPixels() override {
        SDL_Surface *s = SDL_GetWindowSurface(_window);
        if (not s)
            panic("Failed to get window surface");

        return {
            s->pixels,
            {s->w, s->h},
            (usize)s->pitch,
            Gfx::BGRA8888,
        };
    }

    void flip(Slice<Math::Recti>) override {
        SDL_UpdateWindowSurface(_window);
    }

    static Events::Key _fromSdlKeycode(SDL_Keycode sdl) {
        // clang-format off
        switch (sdl) {
            case SDLK_a: return Events::Key::A;
            case SDLK_b: return Events::Key::B;
            case SDLK_c: return Events::Key::C;
            case SDLK_d: return Events::Key::D;
            case SDLK_e: return Events::Key::E;
            case SDLK_f: return Events::Key::F;
            case SDLK_g: return Events::Key::G;
            case SDLK_h: return Events::Key::H;
            case SDLK_i: return Events::Key::I;
            case SDLK_j: return Events::Key::J;
            case SDLK_k: return Events::Key::K;
            case SDLK_l: return Events::Key::L;
            case SDLK_m: return Events::Key::M;
            case SDLK_n: return Events::Key::N;
            case SDLK_o: return Events::Key::O;
            case SDLK_p: return Events::Key::P;
            case SDLK_q: return Events::Key::Q;
            case SDLK_r: return Events::Key::R;
            case SDLK_s: return Events::Key::S;
            case SDLK_t: return Events::Key::T;
            case SDLK_u: return Events::Key::U;
            case SDLK_v: return Events::Key::V;
            case SDLK_w: return Events::Key::W;
            case SDLK_x: return Events::Key::X;
            case SDLK_y: return Events::Key::Y;
            case SDLK_z: return Events::Key::Z;

            case SDLK_1: return Events::Key::NUM1;
            case SDLK_2: return Events::Key::NUM2;
            case SDLK_3: return Events::Key::NUM3;
            case SDLK_4: return Events::Key::NUM4;
            case SDLK_5: return Events::Key::NUM5;
            case SDLK_6: return Events::Key::NUM6;
            case SDLK_7: return Events::Key::NUM7;
            case SDLK_8: return Events::Key::NUM8;
            case SDLK_9: return Events::Key::NUM9;
            case SDLK_0: return Events::Key::NUM0;

            case SDLK_RETURN: return Events::Key::ENTER;
            case SDLK_ESCAPE: return Events::Key::ESC;
            case SDLK_BACKSPACE: return Events::Key::BKSPC;
            case SDLK_TAB: return Events::Key::TAB;
            case SDLK_SPACE: return Events::Key::SPACE;
            case SDLK_CAPSLOCK: return Events::Key::CAPSLOCK;

            case SDLK_F1: return Events::Key::F1;
            case SDLK_F2: return Events::Key::F2;
            case SDLK_F3: return Events::Key::F3;
            case SDLK_F4: return Events::Key::F4;
            case SDLK_F5: return Events::Key::F5;
            case SDLK_F6: return Events::Key::F6;
            case SDLK_F7: return Events::Key::F7;
            case SDLK_F8: return Events::Key::F8;
            case SDLK_F9: return Events::Key::F9;
            case SDLK_F10: return Events::Key::F10;
            case SDLK_F11: return Events::Key::F11;
            case SDLK_F12: return Events::Key::F12;

            case SDLK_INSERT: return Events::Key::INSERT;
            case SDLK_HOME: return Events::Key::HOME;
            case SDLK_PAGEUP: return Events::Key::PGUP;
            case SDLK_DELETE: return Events::Key::DELETE;
            case SDLK_END: return Events::Key::END;
            case SDLK_PAGEDOWN: return Events::Key::PGDOWN;
            case SDLK_RIGHT: return Events::Key::RIGHT;
            case SDLK_LEFT: return Events::Key::LEFT;
            case SDLK_DOWN: return Events::Key::DOWN;
            case SDLK_UP: return Events::Key::UP;

            case SDLK_KP_ENTER: return Events::Key::KPADENTER;
            case SDLK_KP_1: return Events::Key::KPAD1;
            case SDLK_KP_2: return Events::Key::KPAD2;
            case SDLK_KP_3: return Events::Key::KPAD3;
            case SDLK_KP_4: return Events::Key::KPAD4;
            case SDLK_KP_5: return Events::Key::KPAD5;
            case SDLK_KP_6: return Events::Key::KPAD6;
            case SDLK_KP_7: return Events::Key::KPAD7;
            case SDLK_KP_8: return Events::Key::KPAD8;
            case SDLK_KP_9: return Events::Key::KPAD9;
            case SDLK_KP_0: return Events::Key::KPAD0;

            case SDLK_MENU: return Events::Key::MENU;

            case SDLK_LCTRL: return Events::Key::LCTRL;
            case SDLK_LSHIFT: return Events::Key::LSHIFT;
            case SDLK_LALT: return Events::Key::LALT;
            case SDLK_LGUI: return Events::Key::LSUPER;

            case SDLK_RCTRL: return Events::Key::RCTRL;
            case SDLK_RSHIFT: return Events::Key::RSHIFT;
            case SDLK_RALT: return Events::Key::RALT;
            case SDLK_RGUI: return Events::Key::RSUPER;

            default: return Events::Key::INVALID;
        }
        // clang-format on
    }

    static Events::Key _fromSdlScancode(SDL_Scancode sdl) {
        // clang-format off
        switch (sdl) {
            case SDL_SCANCODE_A: return Events::Key::A;
            case SDL_SCANCODE_B: return Events::Key::B;
            case SDL_SCANCODE_C: return Events::Key::C;
            case SDL_SCANCODE_D: return Events::Key::D;
            case SDL_SCANCODE_E: return Events::Key::E;
            case SDL_SCANCODE_F: return Events::Key::F;
            case SDL_SCANCODE_G: return Events::Key::G;
            case SDL_SCANCODE_H: return Events::Key::H;
            case SDL_SCANCODE_I: return Events::Key::I;
            case SDL_SCANCODE_J: return Events::Key::J;
            case SDL_SCANCODE_K: return Events::Key::K;
            case SDL_SCANCODE_L: return Events::Key::L;
            case SDL_SCANCODE_M: return Events::Key::M;
            case SDL_SCANCODE_N: return Events::Key::N;
            case SDL_SCANCODE_O: return Events::Key::O;
            case SDL_SCANCODE_P: return Events::Key::P;
            case SDL_SCANCODE_Q: return Events::Key::Q;
            case SDL_SCANCODE_R: return Events::Key::R;
            case SDL_SCANCODE_S: return Events::Key::S;
            case SDL_SCANCODE_T: return Events::Key::T;
            case SDL_SCANCODE_U: return Events::Key::U;
            case SDL_SCANCODE_V: return Events::Key::V;
            case SDL_SCANCODE_W: return Events::Key::W;
            case SDL_SCANCODE_X: return Events::Key::X;
            case SDL_SCANCODE_Y: return Events::Key::Y;
            case SDL_SCANCODE_Z: return Events::Key::Z;

            case SDL_SCANCODE_1: return Events::Key::NUM1;
            case SDL_SCANCODE_2: return Events::Key::NUM2;
            case SDL_SCANCODE_3: return Events::Key::NUM3;
            case SDL_SCANCODE_4: return Events::Key::NUM4;
            case SDL_SCANCODE_5: return Events::Key::NUM5;
            case SDL_SCANCODE_6: return Events::Key::NUM6;
            case SDL_SCANCODE_7: return Events::Key::NUM7;
            case SDL_SCANCODE_8: return Events::Key::NUM8;
            case SDL_SCANCODE_9: return Events::Key::NUM9;
            case SDL_SCANCODE_0: return Events::Key::NUM0;

            case SDL_SCANCODE_RETURN: return Events::Key::ENTER;
            case SDL_SCANCODE_ESCAPE: return Events::Key::ESC;
            case SDL_SCANCODE_BACKSPACE: return Events::Key::BKSPC;
            case SDL_SCANCODE_TAB: return Events::Key::TAB;
            case SDL_SCANCODE_SPACE: return Events::Key::SPACE;
            case SDL_SCANCODE_CAPSLOCK: return Events::Key::CAPSLOCK;

            case SDL_SCANCODE_F1: return Events::Key::F1;
            case SDL_SCANCODE_F2: return Events::Key::F2;
            case SDL_SCANCODE_F3: return Events::Key::F3;
            case SDL_SCANCODE_F4: return Events::Key::F4;
            case SDL_SCANCODE_F5: return Events::Key::F5;
            case SDL_SCANCODE_F6: return Events::Key::F6;
            case SDL_SCANCODE_F7: return Events::Key::F7;
            case SDL_SCANCODE_F8: return Events::Key::F8;
            case SDL_SCANCODE_F9: return Events::Key::F9;
            case SDL_SCANCODE_F10: return Events::Key::F10;
            case SDL_SCANCODE_F11: return Events::Key::F11;
            case SDL_SCANCODE_F12: return Events::Key::F12;

            case SDL_SCANCODE_INSERT: return Events::Key::INSERT;
            case SDL_SCANCODE_HOME: return Events::Key::HOME;
            case SDL_SCANCODE_PAGEUP: return Events::Key::PGUP;
            case SDL_SCANCODE_DELETE: return Events::Key::DELETE;
            case SDL_SCANCODE_END: return Events::Key::END;
            case SDL_SCANCODE_PAGEDOWN: return Events::Key::PGDOWN;
            case SDL_SCANCODE_RIGHT: return Events::Key::RIGHT;
            case SDL_SCANCODE_LEFT: return Events::Key::LEFT;
            case SDL_SCANCODE_DOWN: return Events::Key::DOWN;
            case SDL_SCANCODE_UP: return Events::Key::UP;

            case SDL_SCANCODE_KP_ENTER: return Events::Key::KPADENTER;
            case SDL_SCANCODE_KP_1: return Events::Key::KPAD1;
            case SDL_SCANCODE_KP_2: return Events::Key::KPAD2;
            case SDL_SCANCODE_KP_3: return Events::Key::KPAD3;
            case SDL_SCANCODE_KP_4: return Events::Key::KPAD4;
            case SDL_SCANCODE_KP_5: return Events::Key::KPAD5;
            case SDL_SCANCODE_KP_6: return Events::Key::KPAD6;
            case SDL_SCANCODE_KP_7: return Events::Key::KPAD7;
            case SDL_SCANCODE_KP_8: return Events::Key::KPAD8;
            case SDL_SCANCODE_KP_9: return Events::Key::KPAD9;
            case SDL_SCANCODE_KP_0: return Events::Key::KPAD0;

            case SDL_SCANCODE_MENU: return Events::Key::MENU;

            case SDL_SCANCODE_LCTRL: return Events::Key::LCTRL;
            case SDL_SCANCODE_LSHIFT: return Events::Key::LSHIFT;
            case SDL_SCANCODE_LALT: return Events::Key::LALT;
            case SDL_SCANCODE_LGUI: return Events::Key::LSUPER;

            case SDL_SCANCODE_RCTRL: return Events::Key::RCTRL;
            case SDL_SCANCODE_RSHIFT: return Events::Key::RSHIFT;
            case SDL_SCANCODE_RALT: return Events::Key::RALT;
            case SDL_SCANCODE_RGUI: return Events::Key::RSUPER;

            default: return Events::Key::INVALID;
        }
        // clang-format on
    }

    static Events::Mod _fromSdlMod(u16 sdl) {
        Events::Mod mod = Events::Mod::NONE;

        if (sdl & KMOD_LSHIFT)
            mod |= Events::Mod::LSHIFT;

        if (sdl & KMOD_RSHIFT)
            mod |= Events::Mod::RSHIFT;

        if (sdl & KMOD_LCTRL)
            mod |= Events::Mod::LCTRL;

        if (sdl & KMOD_RCTRL)
            mod |= Events::Mod::RCTRL;

        if (sdl & KMOD_LALT)
            mod |= Events::Mod::LALT;

        if (sdl & KMOD_RALT)
            mod |= Events::Mod::RALT;

        if (sdl & KMOD_LGUI)
            mod |= Events::Mod::LSUPER;

        if (sdl & KMOD_RGUI)
            mod |= Events::Mod::RSUPER;

        if (sdl & KMOD_NUM)
            mod |= Events::Mod::NUM;

        if (sdl & KMOD_CAPS)
            mod |= Events::Mod::CAPS;

        if (sdl & KMOD_MODE)
            mod |= Events::Mod::MODE;

        if (sdl & KMOD_SCROLL)
            mod |= Events::Mod::SCROLL;

        return mod;
    }

    static Events::KeyboardEvent _fromSdlKeyboardEvent(SDL_KeyboardEvent const &sdl) {
        Events::KeyboardEvent ev{};
        ev.key = _fromSdlKeycode(sdl.keysym.sym);
        ev.code = _fromSdlScancode(sdl.keysym.scancode);
        ev.mods = _fromSdlMod(sdl.keysym.mod);
        return ev;
    }

    SDL_Cursor *_cursor{};
    SDL_SystemCursor _systemCursor{};

    void translate(SDL_Event const &sdlEvent) {
        switch (sdlEvent.type) {
        case SDL_WINDOWEVENT:
            switch (sdlEvent.window.event) {

            case SDL_WINDOWEVENT_RESIZED:
                _shouldLayout = true;
                break;

            case SDL_WINDOWEVENT_EXPOSED:
                _dirty.pushBack(pixels().bound());
                break;
            }
            break;

        case SDL_KEYDOWN: {
            auto ev = _fromSdlKeyboardEvent(sdlEvent.key);
            ev.type = Events::KeyboardEvent::PRESS;
            event<Events::KeyboardEvent>(*this, ev);
            break;
        }

        case SDL_KEYUP: {
            auto ev = _fromSdlKeyboardEvent(sdlEvent.key);
            ev.type = Events::KeyboardEvent::RELEASE;
            event<Events::KeyboardEvent>(*this, ev);
            break;
        }

        case SDL_TEXTINPUT: {
            Str text = sdlEvent.text.text;
            for (Rune r : iterRunes(text)) {
                event<Events::TypedEvent>(*this, r);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID)
                return;

            Math::Vec2<i32> screenPos = {};
            SDL_GetGlobalMouseState(&screenPos.x, &screenPos.y);

            auto buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            // do the hit test and update the cursor

            SDL_Point p = {sdlEvent.motion.x, sdlEvent.motion.y};
            SDL_HitTestResult result = _hitTestCallback(_window, &p, this);
            SDL_SystemCursor systemCursor = SDL_SYSTEM_CURSOR_ARROW;

            switch (result) {
            case SDL_HITTEST_RESIZE_TOPLEFT:
            case SDL_HITTEST_RESIZE_BOTTOMRIGHT:
                systemCursor = SDL_SYSTEM_CURSOR_SIZENWSE;
                break;
            case SDL_HITTEST_RESIZE_TOPRIGHT:
            case SDL_HITTEST_RESIZE_BOTTOMLEFT:
                systemCursor = SDL_SYSTEM_CURSOR_SIZENESW;
                break;
            case SDL_HITTEST_RESIZE_TOP:
            case SDL_HITTEST_RESIZE_BOTTOM:
                systemCursor = SDL_SYSTEM_CURSOR_SIZENS;
                break;
            case SDL_HITTEST_RESIZE_LEFT:
            case SDL_HITTEST_RESIZE_RIGHT:
                systemCursor = SDL_SYSTEM_CURSOR_SIZEWE;
                break;
            default:
                break;
            }

            if (_systemCursor != systemCursor) {
                if (_cursor) {
                    SDL_FreeCursor(_cursor);
                }

                _cursor = SDL_CreateSystemCursor(systemCursor);
                SDL_SetCursor(_cursor);
                _systemCursor = systemCursor;
            }

            _lastMousePos = {
                sdlEvent.motion.x,
                sdlEvent.motion.y,
            };

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::MOVE,
                    .pos = _lastMousePos,
                    .delta = screenPos - _lastScreenMousePos,
                    .buttons = buttons,
                }
            );

            _lastScreenMousePos = screenPos.cast<isize>();
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            Events::Button button = Events::Button::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = Events::Button::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = Events::Button::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = Events::Button::MIDDLE;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::RELEASE,
                    .pos = _lastMousePos,
                    .buttons = buttons,
                    .button = button,
                }
            );
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            Events::Button button = Events::Button::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = Events::Button::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = Events::Button::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = Events::Button::MIDDLE;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::PRESS,
                    .pos = _lastMousePos,
                    .buttons = buttons,
                    .button = button,
                }
            );
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (sdlEvent.wheel.which == SDL_TOUCH_MOUSEID)
                return;

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::SCROLL,
                    .pos = _lastMousePos,
                    .scroll = {
#if SDL_VERSION_ATLEAST(2, 0, 18)
                        -sdlEvent.wheel.preciseX,
                        sdlEvent.wheel.preciseY,
#else
                        -(f64)sdlEvent.wheel.x,
                        (f64)sdlEvent.wheel.y,
#endif
                    },
                }
            );

            break;
        }

        case SDL_QUIT: {
            bubble<Events::RequestExitEvent>(*this);
            break;
        }

        default:
            break;
        }
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.scale(dpi());
        Host::paint(g, r);
        g.restore();
    }

    Res<> wait(TimeStamp ts) override {
        // HACK: Since we don't have a lot of control onto how SDL wait for
        //       events we can't integrate it properly with our event loop
        //       To remedi this we will just cap how long we wait, this way
        //       we can poll for event.

        // NOTE: A better option would be to have SDL in a separeted thread
        //       and do the communication over an inter-thread channel but
        //       but this would require to make the Framework thread safe
        auto delay = TimeSpan::fromMSecs((usize)(FRAME_TIME * 1000));
        auto cappedWait = min(ts, Sys::now() + delay);
        try$(Sys::globalSched().wait(cappedWait));

        SDL_Event e{};
        while (SDL_PollEvent(&e) != 0 and alive())
            translate(e);
        return Ok();
    }

    void bubble(Sys::Event &event) override {
        if (auto *e = event.is<DragEvent>()) {
            if (e->type == DragEvent::START) {
                SDL_CaptureMouse(SDL_TRUE);
            } else if (e->type == DragEvent::END) {
                SDL_CaptureMouse(SDL_FALSE);
            } else if (e->type == DragEvent::DRAG) {
                Math::Vec2<i32> pos{};
                SDL_GetWindowPosition(_window, &pos.x, &pos.y);
                pos = pos + e->delta.cast<i32>();
                SDL_SetWindowPosition(_window, pos.x, pos.y);
            }
            event.accept();
        } else if (event.is<Events::RequestMinimizeEvent>()) {
            SDL_MinimizeWindow(_window);
            event.accept();
        } else if (event.is<Events::RequestMaximizeEvent>()) {
            if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MAXIMIZED)
                SDL_RestoreWindow(_window);
            else
                SDL_MaximizeWindow(_window);
            event.accept();
        }

        Host::bubble(event);
    }
};

static SDL_HitTestResult _hitTestCallback(SDL_Window *window, SDL_Point const *area, void *data) {
    SdlHost *host = (SdlHost *)data;
    isize grabPadding = 6 * host->dpi();
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    if (area->y < grabPadding) {
        if (area->x < grabPadding)
            return SDL_HITTEST_RESIZE_TOPLEFT;
        else if (area->x > width - grabPadding)
            return SDL_HITTEST_RESIZE_TOPRIGHT;

        return SDL_HITTEST_RESIZE_TOP;
    } else if (area->y > height - grabPadding) {
        if (area->x < grabPadding)
            return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        else if (area->x > width - grabPadding)
            return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

        return SDL_HITTEST_RESIZE_BOTTOM;
    } else if (area->x < grabPadding) {
        return SDL_HITTEST_RESIZE_LEFT;
    } else if (area->x > width - grabPadding) {
        return SDL_HITTEST_RESIZE_RIGHT;
    }

    return SDL_HITTEST_NORMAL;
}

Res<Strong<Host>> makeHost(Child root) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto size = root->size({700, 500}, Hint::MIN);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
    );

    if (not window)
        return Error::other(SDL_GetError());

    auto host = makeStrong<SdlHost>(root, window);

    SDL_SetWindowHitTest(window, _hitTestCallback, (void *)&host.unwrap());

    return Ok(host);
}

} // namespace Karm::Ui::_Embed
