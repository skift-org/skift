#include <SDL.h>
#include <karm-ui/drag.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

struct SdlHost :
    public Ui::Host {
    SDL_Window *_window{};

    Math::Vec2i _lastMousePos{};
    Math::Vec2i _lastScreenMousePos{};

    SdlHost(Ui::Child root, SDL_Window *window)
        : Ui::Host(root), _window(window) {
    }

    ~SdlHost() {
        SDL_DestroyWindow(_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Quit();
    }

    Gfx::MutPixels mutPixels() override {
        SDL_Surface *s = SDL_GetWindowSurface(_window);

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
            event<Events::TypedEvent>(*this, Events::KeyboardEvent::PRESS);
            break;
        }

        case SDL_KEYUP: {
            event<Events::TypedEvent>(*this, Events::KeyboardEvent::RELEASE);
            break;
        }

        case SDL_TEXTINPUT: {
            Str text = sdlEvent.text.text;
            for (u8 c : iterRunes(text)) {
                logInfo("typed: {c}", c);
                event<Events::TypedEvent>(*this, c);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Math::Vec2<i32> screenPos = {};
            SDL_GetGlobalMouseState(&screenPos.x, &screenPos.y);

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            _lastMousePos = {sdlEvent.motion.x, sdlEvent.motion.y};

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::MOVE,
                    .pos = _lastMousePos,
                    .delta = screenPos - _lastScreenMousePos,
                    .buttons = buttons,
                });

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
                });
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
                });
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (sdlEvent.wheel.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::SCROLL,
                    .pos = _lastMousePos,
                    .scroll = {
#if SDL_VERSION_ATLEAST(2, 0, 18)
                        sdlEvent.wheel.preciseX,
                        sdlEvent.wheel.preciseY,
#else
                        (f64)sdlEvent.wheel.x,
                        (f64)sdlEvent.wheel.y,
#endif
                    },
                });

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

    Res<> wait(TimeStamp ts) override {
        // HACK: Since we don't have a lot of control onto how SDL wait for
        //       events we can't integrate it properly with our event loop
        //       To remedi this we will just cap how long we wait, this way
        //       we can poll for event.

        // NOTE: A better option would be to have SDL in a separeted thread
        //       and do the communication over an inter-thread channel but
        //       but this would require to make the Framework thread safe
        auto delay = TimeSpan::fromMSecs((usize)(FRAME_TIME * 1000 * 2));
        auto cappedWait = min(ts, Sys::now() + delay);
        try$(Sys::globalSched().wait(cappedWait));

        SDL_Event e{};
        while (SDL_PollEvent(&e) != 0 and alive())
            translate(e);
        return Ok();
    }

    void bubble(Sys::Event &e) override {
        if (e.is<Ui::DragEvent>()) {
            auto &dragEvent = e.unwrap<Ui::DragEvent>();
            if (dragEvent.type == Ui::DragEvent::START) {
                SDL_CaptureMouse(SDL_TRUE);
            } else if (dragEvent.type == Ui::DragEvent::END) {
                SDL_CaptureMouse(SDL_FALSE);
            } else if (dragEvent.type == Ui::DragEvent::DRAG) {
                Math::Vec2<i32> pos{};
                SDL_GetWindowPosition(_window, &pos.x, &pos.y);
                pos = pos + dragEvent.delta.cast<i32>();
                SDL_SetWindowPosition(_window, pos.x, pos.y);
            }
        }

        Ui::Host::bubble(e);
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto size = root->size({700, 500}, Layout::Hint::MIN);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);

    if (not window)
        return Error::other(SDL_GetError());

    return Ok(makeStrong<SdlHost>(root, window));
}

} // namespace Karm::Ui::_Embed
