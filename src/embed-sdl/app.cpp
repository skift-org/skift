#include <SDL.h>
#include <embed/app.h>

namespace Embed {

struct SdlHost : public App::Host {
    SDL_Window *_window{};
    Math::Vec2i _lastMousePos{};

    SdlHost(SDL_Window *window)
        : _window(window) {
    }

    ~SdlHost() {
        SDL_DestroyWindow(_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Quit();
    }

    Gfx::Surface surface() override {
        SDL_Surface *s = SDL_GetWindowSurface(_window);

        return {
            Gfx::BGRA8888,
            s->pixels,
            s->w,
            s->h,
            s->pitch,
        };
    }

    void flip(Slice<Math::Recti> rects) override {
        // Math::Recti and SDL_Rect have the same layout so this is fine
        SDL_UpdateWindowSurfaceRects(
            _window,
            reinterpret_cast<const SDL_Rect *>(rects.buf()),
            rects.len());
    }

    void dispatch(SDL_Event const &e) {
        switch (e.type) {
        case SDL_WINDOWEVENT:
            break;

        case SDL_KEYDOWN: {
            Events::KeyboardEvent event{
                .type = Events::KeyboardEvent::PRESS,
            };
            handle(event);
            break;
        }

        case SDL_KEYUP: {
            Events::KeyboardEvent event{
                .type = Events::KeyboardEvent::RELEASE,
            };
            handle(event);
            break;
        }

        case SDL_MOUSEMOTION: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent event{
                .type = Events::MouseEvent::MOVE,

                .pos = {e.motion.x, e.motion.y},
                .delta = {e.motion.xrel, e.motion.yrel},
            };

            event.buttons |= (e.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            event.buttons |= (e.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            event.buttons |= (e.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            _lastMousePos = event.pos;

            handle(event);
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent event{
                .type = Events::MouseEvent::RELEASE,
                .pos = _lastMousePos,
            };

            event.button = (e.button.which == SDL_BUTTON_LEFT) ? Events::Button::LEFT : Events::Button::NONE;
            event.button = (e.button.which == SDL_BUTTON_RIGHT) ? Events::Button::MIDDLE : Events::Button::NONE;
            event.button = (e.button.which == SDL_BUTTON_MIDDLE) ? Events::Button::RIGHT : Events::Button::NONE;

            event.buttons |= (e.button.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            handle(event);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent event{
                .type = Events::MouseEvent::PRESS,
                .pos = _lastMousePos,
            };

            event.button = (e.button.button == SDL_BUTTON_LEFT) ? Events::Button::LEFT : Events::Button::NONE;
            event.button = (e.button.button == SDL_BUTTON_RIGHT) ? Events::Button::MIDDLE : Events::Button::NONE;
            event.button = (e.button.button == SDL_BUTTON_MIDDLE) ? Events::Button::RIGHT : Events::Button::NONE;

            event.buttons |= (e.button.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            handle(event);
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (e.wheel.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::MouseEvent event{
                .type = Events::MouseEvent::SCROLL,
                .pos = _lastMousePos,
                .scroll = {e.wheel.x, e.wheel.y},
            };

            handle(event);
            break;
        }

        case SDL_QUIT:
            exit();
            break;

        default:
            break;
        }
    }

    void pump() override {
        SDL_Event e{};

        while (SDL_PollEvent(&e) != 0 && _alive) {
            dispatch(e);
        }
    }

    void wait(size_t ms) override {
        SDL_WaitEventTimeout(nullptr, ms);
    }
};

Result<Strong<Karm::App::Host>> makeHost() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        1136,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (!window) {
        return Error{SDL_GetError()};
    }

    return {makeStrong<SdlHost>(window)};
}

} // namespace Embed
