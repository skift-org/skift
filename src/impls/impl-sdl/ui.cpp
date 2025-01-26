#include <SDL.h>
#include <karm-image/loader.h>
#include <karm-pkg/bundle.h>
#include <karm-ui/_embed.h>
#include <karm-ui/drag.h>

namespace Karm::Ui::_Embed {

static SDL_HitTestResult _hitTestCallback(SDL_Window* window, SDL_Point const* area, void* data);

struct SdlHost :
    public Host {
    SDL_Window* _window{};

    Math::Vec2i _lastMousePos{};
    Math::Vec2i _lastScreenMousePos{};

    SdlHost(Child root, SDL_Window* window)
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
        SDL_Surface* s = SDL_GetWindowSurface(_window);
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

    static App::Key _fromSdlKeycode(SDL_Keycode sdl) {
        // clang-format off
        switch (sdl) {
            case SDLK_a: return App::Key::A;
            case SDLK_b: return App::Key::B;
            case SDLK_c: return App::Key::C;
            case SDLK_d: return App::Key::D;
            case SDLK_e: return App::Key::E;
            case SDLK_f: return App::Key::F;
            case SDLK_g: return App::Key::G;
            case SDLK_h: return App::Key::H;
            case SDLK_i: return App::Key::I;
            case SDLK_j: return App::Key::J;
            case SDLK_k: return App::Key::K;
            case SDLK_l: return App::Key::L;
            case SDLK_m: return App::Key::M;
            case SDLK_n: return App::Key::N;
            case SDLK_o: return App::Key::O;
            case SDLK_p: return App::Key::P;
            case SDLK_q: return App::Key::Q;
            case SDLK_r: return App::Key::R;
            case SDLK_s: return App::Key::S;
            case SDLK_t: return App::Key::T;
            case SDLK_u: return App::Key::U;
            case SDLK_v: return App::Key::V;
            case SDLK_w: return App::Key::W;
            case SDLK_x: return App::Key::X;
            case SDLK_y: return App::Key::Y;
            case SDLK_z: return App::Key::Z;

            case SDLK_1: return App::Key::NUM1;
            case SDLK_2: return App::Key::NUM2;
            case SDLK_3: return App::Key::NUM3;
            case SDLK_4: return App::Key::NUM4;
            case SDLK_5: return App::Key::NUM5;
            case SDLK_6: return App::Key::NUM6;
            case SDLK_7: return App::Key::NUM7;
            case SDLK_8: return App::Key::NUM8;
            case SDLK_9: return App::Key::NUM9;
            case SDLK_0: return App::Key::NUM0;

            case SDLK_RETURN: return App::Key::ENTER;
            case SDLK_ESCAPE: return App::Key::ESC;
            case SDLK_BACKSPACE: return App::Key::BKSPC;
            case SDLK_TAB: return App::Key::TAB;
            case SDLK_SPACE: return App::Key::SPACE;
            case SDLK_CAPSLOCK: return App::Key::CAPSLOCK;

            case SDLK_F1: return App::Key::F1;
            case SDLK_F2: return App::Key::F2;
            case SDLK_F3: return App::Key::F3;
            case SDLK_F4: return App::Key::F4;
            case SDLK_F5: return App::Key::F5;
            case SDLK_F6: return App::Key::F6;
            case SDLK_F7: return App::Key::F7;
            case SDLK_F8: return App::Key::F8;
            case SDLK_F9: return App::Key::F9;
            case SDLK_F10: return App::Key::F10;
            case SDLK_F11: return App::Key::F11;
            case SDLK_F12: return App::Key::F12;

            case SDLK_INSERT: return App::Key::INSERT;
            case SDLK_HOME: return App::Key::HOME;
            case SDLK_PAGEUP: return App::Key::PGUP;
            case SDLK_DELETE: return App::Key::DELETE;
            case SDLK_END: return App::Key::END;
            case SDLK_PAGEDOWN: return App::Key::PGDOWN;
            case SDLK_RIGHT: return App::Key::RIGHT;
            case SDLK_LEFT: return App::Key::LEFT;
            case SDLK_DOWN: return App::Key::DOWN;
            case SDLK_UP: return App::Key::UP;

            case SDLK_KP_ENTER: return App::Key::KPADENTER;
            case SDLK_KP_1: return App::Key::KPAD1;
            case SDLK_KP_2: return App::Key::KPAD2;
            case SDLK_KP_3: return App::Key::KPAD3;
            case SDLK_KP_4: return App::Key::KPAD4;
            case SDLK_KP_5: return App::Key::KPAD5;
            case SDLK_KP_6: return App::Key::KPAD6;
            case SDLK_KP_7: return App::Key::KPAD7;
            case SDLK_KP_8: return App::Key::KPAD8;
            case SDLK_KP_9: return App::Key::KPAD9;
            case SDLK_KP_0: return App::Key::KPAD0;

            case SDLK_MENU: return App::Key::MENU;

            case SDLK_LCTRL: return App::Key::LCTRL;
            case SDLK_LSHIFT: return App::Key::LSHIFT;
            case SDLK_LALT: return App::Key::LALT;
            case SDLK_LGUI: return App::Key::LSUPER;

            case SDLK_RCTRL: return App::Key::RCTRL;
            case SDLK_RSHIFT: return App::Key::RSHIFT;
            case SDLK_RALT: return App::Key::RALT;
            case SDLK_RGUI: return App::Key::RSUPER;

            default: return App::Key::INVALID;
        }
        // clang-format on
    }

    static App::Key _fromSdlScancode(SDL_Scancode sdl) {
        // clang-format off
        switch (sdl) {
            case SDL_SCANCODE_A: return App::Key::A;
            case SDL_SCANCODE_B: return App::Key::B;
            case SDL_SCANCODE_C: return App::Key::C;
            case SDL_SCANCODE_D: return App::Key::D;
            case SDL_SCANCODE_E: return App::Key::E;
            case SDL_SCANCODE_F: return App::Key::F;
            case SDL_SCANCODE_G: return App::Key::G;
            case SDL_SCANCODE_H: return App::Key::H;
            case SDL_SCANCODE_I: return App::Key::I;
            case SDL_SCANCODE_J: return App::Key::J;
            case SDL_SCANCODE_K: return App::Key::K;
            case SDL_SCANCODE_L: return App::Key::L;
            case SDL_SCANCODE_M: return App::Key::M;
            case SDL_SCANCODE_N: return App::Key::N;
            case SDL_SCANCODE_O: return App::Key::O;
            case SDL_SCANCODE_P: return App::Key::P;
            case SDL_SCANCODE_Q: return App::Key::Q;
            case SDL_SCANCODE_R: return App::Key::R;
            case SDL_SCANCODE_S: return App::Key::S;
            case SDL_SCANCODE_T: return App::Key::T;
            case SDL_SCANCODE_U: return App::Key::U;
            case SDL_SCANCODE_V: return App::Key::V;
            case SDL_SCANCODE_W: return App::Key::W;
            case SDL_SCANCODE_X: return App::Key::X;
            case SDL_SCANCODE_Y: return App::Key::Y;
            case SDL_SCANCODE_Z: return App::Key::Z;

            case SDL_SCANCODE_1: return App::Key::NUM1;
            case SDL_SCANCODE_2: return App::Key::NUM2;
            case SDL_SCANCODE_3: return App::Key::NUM3;
            case SDL_SCANCODE_4: return App::Key::NUM4;
            case SDL_SCANCODE_5: return App::Key::NUM5;
            case SDL_SCANCODE_6: return App::Key::NUM6;
            case SDL_SCANCODE_7: return App::Key::NUM7;
            case SDL_SCANCODE_8: return App::Key::NUM8;
            case SDL_SCANCODE_9: return App::Key::NUM9;
            case SDL_SCANCODE_0: return App::Key::NUM0;

            case SDL_SCANCODE_RETURN: return App::Key::ENTER;
            case SDL_SCANCODE_ESCAPE: return App::Key::ESC;
            case SDL_SCANCODE_BACKSPACE: return App::Key::BKSPC;
            case SDL_SCANCODE_TAB: return App::Key::TAB;
            case SDL_SCANCODE_SPACE: return App::Key::SPACE;
            case SDL_SCANCODE_CAPSLOCK: return App::Key::CAPSLOCK;

            case SDL_SCANCODE_F1: return App::Key::F1;
            case SDL_SCANCODE_F2: return App::Key::F2;
            case SDL_SCANCODE_F3: return App::Key::F3;
            case SDL_SCANCODE_F4: return App::Key::F4;
            case SDL_SCANCODE_F5: return App::Key::F5;
            case SDL_SCANCODE_F6: return App::Key::F6;
            case SDL_SCANCODE_F7: return App::Key::F7;
            case SDL_SCANCODE_F8: return App::Key::F8;
            case SDL_SCANCODE_F9: return App::Key::F9;
            case SDL_SCANCODE_F10: return App::Key::F10;
            case SDL_SCANCODE_F11: return App::Key::F11;
            case SDL_SCANCODE_F12: return App::Key::F12;

            case SDL_SCANCODE_INSERT: return App::Key::INSERT;
            case SDL_SCANCODE_HOME: return App::Key::HOME;
            case SDL_SCANCODE_PAGEUP: return App::Key::PGUP;
            case SDL_SCANCODE_DELETE: return App::Key::DELETE;
            case SDL_SCANCODE_END: return App::Key::END;
            case SDL_SCANCODE_PAGEDOWN: return App::Key::PGDOWN;
            case SDL_SCANCODE_RIGHT: return App::Key::RIGHT;
            case SDL_SCANCODE_LEFT: return App::Key::LEFT;
            case SDL_SCANCODE_DOWN: return App::Key::DOWN;
            case SDL_SCANCODE_UP: return App::Key::UP;

            case SDL_SCANCODE_KP_ENTER: return App::Key::KPADENTER;
            case SDL_SCANCODE_KP_1: return App::Key::KPAD1;
            case SDL_SCANCODE_KP_2: return App::Key::KPAD2;
            case SDL_SCANCODE_KP_3: return App::Key::KPAD3;
            case SDL_SCANCODE_KP_4: return App::Key::KPAD4;
            case SDL_SCANCODE_KP_5: return App::Key::KPAD5;
            case SDL_SCANCODE_KP_6: return App::Key::KPAD6;
            case SDL_SCANCODE_KP_7: return App::Key::KPAD7;
            case SDL_SCANCODE_KP_8: return App::Key::KPAD8;
            case SDL_SCANCODE_KP_9: return App::Key::KPAD9;
            case SDL_SCANCODE_KP_0: return App::Key::KPAD0;

            case SDL_SCANCODE_MENU: return App::Key::MENU;

            case SDL_SCANCODE_LCTRL: return App::Key::LCTRL;
            case SDL_SCANCODE_LSHIFT: return App::Key::LSHIFT;
            case SDL_SCANCODE_LALT: return App::Key::LALT;
            case SDL_SCANCODE_LGUI: return App::Key::LSUPER;

            case SDL_SCANCODE_RCTRL: return App::Key::RCTRL;
            case SDL_SCANCODE_RSHIFT: return App::Key::RSHIFT;
            case SDL_SCANCODE_RALT: return App::Key::RALT;
            case SDL_SCANCODE_RGUI: return App::Key::RSUPER;

            default: return App::Key::INVALID;
        }
        // clang-format on
    }

    static App::KeyMod _fromSdlMod(u16 sdl) {
        App::KeyMod mod = App::KeyMod::NONE;

        if (sdl & KMOD_LSHIFT)
            mod |= App::KeyMod::LSHIFT;

        if (sdl & KMOD_RSHIFT)
            mod |= App::KeyMod::RSHIFT;

        if (sdl & KMOD_LCTRL)
            mod |= App::KeyMod::LCTRL;

        if (sdl & KMOD_RCTRL)
            mod |= App::KeyMod::RCTRL;

        if (sdl & KMOD_LALT)
            mod |= App::KeyMod::LALT;

        if (sdl & KMOD_RALT)
            mod |= App::KeyMod::RALT;

        if (sdl & KMOD_LGUI)
            mod |= App::KeyMod::LSUPER;

        if (sdl & KMOD_RGUI)
            mod |= App::KeyMod::RSUPER;

        if (sdl & KMOD_NUM)
            mod |= App::KeyMod::NUM;

        if (sdl & KMOD_CAPS)
            mod |= App::KeyMod::CAPS;

        if (sdl & KMOD_MODE)
            mod |= App::KeyMod::MODE;

        if (sdl & KMOD_SCROLL)
            mod |= App::KeyMod::SCROLL;

        return mod;
    }

    static App::KeyboardEvent _fromSdlKeyboardEvent(SDL_KeyboardEvent const& sdl) {
        App::KeyboardEvent ev{};
        ev.key = _fromSdlKeycode(sdl.keysym.sym);
        ev.code = _fromSdlScancode(sdl.keysym.scancode);
        ev.mods = _fromSdlMod(sdl.keysym.mod);
        return ev;
    }

    SDL_Cursor* _cursor{};
    SDL_SystemCursor _systemCursor{};

    void translate(SDL_Event const& sdlEvent) {
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
            ev.type = App::KeyboardEvent::PRESS;
            event<App::KeyboardEvent>(*this, ev);
            break;
        }

        case SDL_KEYUP: {
            auto ev = _fromSdlKeyboardEvent(sdlEvent.key);
            ev.type = App::KeyboardEvent::RELEASE;
            event<App::KeyboardEvent>(*this, ev);
            break;
        }

        case SDL_TEXTINPUT: {
            Str text = sdlEvent.text.text;
            for (Rune r : iterRunes(text)) {
                event<App::TypeEvent>(*this, r);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID)
                return;

            Math::Vec2<i32> screenPos = {};
            SDL_GetGlobalMouseState(&screenPos.x, &screenPos.y);

            auto buttons = App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? App::MouseButton::LEFT : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? App::MouseButton::MIDDLE : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? App::MouseButton::RIGHT : App::MouseButton::NONE;

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

            event<App::MouseEvent>(
                *this,
                App::MouseEvent{
                    .type = App::MouseEvent::MOVE,
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

            App::MouseButton buttons = App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? App::MouseButton::LEFT : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? App::MouseButton::MIDDLE : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? App::MouseButton::RIGHT : App::MouseButton::NONE;

            App::MouseButton button = App::MouseButton::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = App::MouseButton::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = App::MouseButton::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = App::MouseButton::MIDDLE;
            }

            event<App::MouseEvent>(
                *this,
                App::MouseEvent{
                    .type = App::MouseEvent::RELEASE,
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

            App::MouseButton buttons = App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? App::MouseButton::LEFT : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? App::MouseButton::MIDDLE : App::MouseButton::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? App::MouseButton::RIGHT : App::MouseButton::NONE;

            App::MouseButton button = App::MouseButton::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = App::MouseButton::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = App::MouseButton::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = App::MouseButton::MIDDLE;
            }

            event<App::MouseEvent>(
                *this,
                App::MouseEvent{
                    .type = App::MouseEvent::PRESS,
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

            event<App::MouseEvent>(
                *this,
                App::MouseEvent{
                    .type = App::MouseEvent::SCROLL,
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
            bubble<App::RequestExitEvent>(*this);
            break;
        }

        default:
            break;
        }
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.scale(dpi());
        Host::paint(g, r);
        g.pop();
    }

    Res<> wait(Instant ts) override {
        // HACK: Since we don't have a lot of control onto how SDL wait for
        //       events we can't integrate it properly with our event loop
        //       To remedi this we will just cap how long we wait, this way
        //       we can poll for event.

        // NOTE: A better option would be to have SDL in a separeted thread
        //       and do the communication over an inter-thread channel but
        //       but this would require to make the Framework thread safe
        auto delay = Duration::fromMSecs((usize)(FRAME_TIME * 1000));
        auto cappedWait = min(ts, Sys::instant() + delay);
        try$(Sys::globalSched().wait(cappedWait));

        SDL_Event e{};
        while (SDL_PollEvent(&e) != 0 and alive())
            translate(e);
        return Ok();
    }

    void bubble(App::Event& event) override {
        if (auto e = event.is<DragEvent>()) {
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
        } else if (event.is<App::RequestMinimizeEvent>()) {
            SDL_MinimizeWindow(_window);
            event.accept();
        } else if (event.is<App::RequestMaximizeEvent>()) {
            if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MAXIMIZED)
                SDL_RestoreWindow(_window);
            else
                SDL_MaximizeWindow(_window);
            event.accept();
        }

        Host::bubble(event);
    }
};

static SDL_HitTestResult _hitTestCallback(SDL_Window* window, SDL_Point const* area, void* data) {
    SdlHost* host = (SdlHost*)data;
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

static Res<> _setWindowIcon(SDL_Window* window) {
    auto url = try$(Pkg::currentBundle()).url() / "images/icon.qoi"_path;
    auto defaultUrl = "bundle://karm-ui/images/icon.qoi"_url;
    auto image = Image::load(url).unwrapOrElse([&] {
        return Image::loadOrFallback(defaultUrl).take();
    });

    auto* surface = SDL_CreateRGBSurface(0, image.width(), image.height(), 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (not surface)
        return Error::other(SDL_GetError());
    Defer defer{[&] {
        SDL_FreeSurface(surface);
    }};

    Gfx::MutPixels pixels{
        surface->pixels,
        {surface->w, surface->h},
        (usize)surface->pitch,
        Gfx::BGRA8888,
    };
    blitUnsafe(pixels, image.pixels());

    SDL_SetWindowIcon(window, surface);

    return Ok();
}

Res<Rc<Host>> makeHost(Child root) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto size = root->size({1024, 720}, Hint::MIN);

    SDL_Window* window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
    );

    if (not window)
        return Error::other(SDL_GetError());

    auto iconRes = _setWindowIcon(window);
    if (not iconRes)
        logWarn("could not set window icon: {}", iconRes);

    auto host = makeRc<SdlHost>(root, window);

    SDL_SetWindowHitTest(window, _hitTestCallback, (void*)&host.unwrap());

    return Ok(host);
}

Async::Task<> runAsync(Sys::Context&, Child) {
    notImplemented();
}

} // namespace Karm::Ui::_Embed
