#include <SDL.h>
#include <embed-ui/ui.h>
#include <karm-ui/drag.h>

namespace Embed {

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
            Events::KeyboardEvent uiEvent{
                .type = Events::KeyboardEvent::PRESS,
            };
            event(uiEvent);
            break;
        }

        case SDL_KEYUP: {
            Events::KeyboardEvent uiEvent{
                .type = Events::KeyboardEvent::RELEASE,
            };
            event(uiEvent);
            break;
        }

        case SDL_MOUSEMOTION: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Math::Vec2<i32> screenPos = {};
            SDL_GetGlobalMouseState(&screenPos.x, &screenPos.y);

            Events::MouseEvent uiEvent{
                .type = Events::MouseEvent::MOVE,

                .pos = {sdlEvent.motion.x, sdlEvent.motion.y},
                .delta = screenPos - _lastScreenMousePos,
            };

            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            _lastMousePos = uiEvent.pos;
            _lastScreenMousePos = screenPos.cast<isize>();

            event(uiEvent);
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent uiEvent{
                .type = Events::MouseEvent::RELEASE,
                .pos = _lastMousePos,
            };

            uiEvent.button = (sdlEvent.button.which == SDL_BUTTON_LEFT) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.button = (sdlEvent.button.which == SDL_BUTTON_RIGHT) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.button = (sdlEvent.button.which == SDL_BUTTON_MIDDLE) ? Events::Button::RIGHT : Events::Button::NONE;

            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            event(uiEvent);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent uiEvent{
                .type = Events::MouseEvent::PRESS,
                .pos = _lastMousePos,
            };

            uiEvent.button = (sdlEvent.button.button == SDL_BUTTON_LEFT) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.button = (sdlEvent.button.button == SDL_BUTTON_RIGHT) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.button = (sdlEvent.button.button == SDL_BUTTON_MIDDLE) ? Events::Button::RIGHT : Events::Button::NONE;

            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.button.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            event(uiEvent);
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (sdlEvent.wheel.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent uiEvent{
                .type = Events::MouseEvent::SCROLL,
                .pos = _lastMousePos,
                .scrollLines = {
                    sdlEvent.wheel.x,
                    sdlEvent.wheel.y,
                },
                .scrollPrecise = {
#if SDL_VERSION_ATLEAST(2, 0, 18)
                    sdlEvent.wheel.preciseX,
                    sdlEvent.wheel.preciseY,
#else
                    (f64)sdlEvent.wheel.x,
                    (f64)sdlEvent.wheel.y,
#endif
                },
            };

            event(uiEvent);

            break;
        }

        case SDL_QUIT: {
            Events::ExitEvent uiEvent{Ok()};
            bubble(uiEvent);
            break;
        }

        default:
            break;
        }
    }

    void pump() override {
        SDL_Event e{};

        while (SDL_PollEvent(&e) != 0 and alive()) {
            translate(e);
        }
    }

    void wait(usize ms) override {
        SDL_WaitEventTimeout(nullptr, ms);
    }

    void bubble(Events::Event &e) override {
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
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS | SDL_WINDOW_UTILITY);

    if (not window) {
        return Error::other(SDL_GetError());
    }

    return Ok(makeStrong<SdlHost>(root, window));
}

} // namespace Embed
