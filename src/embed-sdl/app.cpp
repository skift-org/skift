#include <SDL.h>
#include <embed/ui.h>

namespace Embed {

struct SdlApp : public Ui::App {
    using Ui::App::dispatch;

    SDL_Window *_window;
    Math::Vec2i _lastMousePos{};

    SdlApp(SDL_Window *window)
        : _window(window) {
    }

    ~SdlApp() {
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
            Ui::KeyboardEvent event{
                .type = Ui::KeyboardEvent::PRESS,
            };
            dispatch(event);
            break;
        }

        case SDL_KEYUP: {
            Ui::KeyboardEvent event{
                .type = Ui::KeyboardEvent::RELEASE,
            };
            dispatch(event);
            break;
        }

        case SDL_MOUSEMOTION: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Ui::MouseEvent event{
                .type = Ui::MouseEvent::MOVE,

                .pos = {e.motion.x, e.motion.y},
                .delta = {e.motion.xrel, e.motion.yrel},
            };

            event.buttons |= (e.motion.state & SDL_BUTTON_LMASK) ? Input::Button::LEFT : Input::Button::NONE;
            event.buttons |= (e.motion.state & SDL_BUTTON_MMASK) ? Input::Button::MIDDLE : Input::Button::NONE;
            event.buttons |= (e.motion.state & SDL_BUTTON_RMASK) ? Input::Button::RIGHT : Input::Button::NONE;

            _lastMousePos = event.pos;

            dispatch(event);
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Ui::MouseEvent event{
                .type = Ui::MouseEvent::RELEASE,
                .pos = _lastMousePos,
            };

            event.button = (e.button.which == SDL_BUTTON_LEFT) ? Input::Button::LEFT : Input::Button::NONE;
            event.button = (e.button.which == SDL_BUTTON_RIGHT) ? Input::Button::MIDDLE : Input::Button::NONE;
            event.button = (e.button.which == SDL_BUTTON_MIDDLE) ? Input::Button::RIGHT : Input::Button::NONE;

            event.buttons |= (e.button.state & SDL_BUTTON_LMASK) ? Input::Button::LEFT : Input::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_MMASK) ? Input::Button::MIDDLE : Input::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_RMASK) ? Input::Button::RIGHT : Input::Button::NONE;

            dispatch(event);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (e.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Ui::MouseEvent event{
                .type = Ui::MouseEvent::PRESS,
                .pos = _lastMousePos,
            };

            event.button = (e.button.button == SDL_BUTTON_LEFT) ? Input::Button::LEFT : Input::Button::NONE;
            event.button = (e.button.button == SDL_BUTTON_RIGHT) ? Input::Button::MIDDLE : Input::Button::NONE;
            event.button = (e.button.button == SDL_BUTTON_MIDDLE) ? Input::Button::RIGHT : Input::Button::NONE;

            event.buttons |= (e.button.state & SDL_BUTTON_LMASK) ? Input::Button::LEFT : Input::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_MMASK) ? Input::Button::MIDDLE : Input::Button::NONE;
            event.buttons |= (e.button.state & SDL_BUTTON_RMASK) ? Input::Button::RIGHT : Input::Button::NONE;

            dispatch(event);
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (e.wheel.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Ui::MouseEvent event{
                .type = Ui::MouseEvent::SCROLL,
                .pos = _lastMousePos,
                .scroll = {e.wheel.x, e.wheel.y},
            };

            dispatch(event);
            break;
        }

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

Result<Strong<Karm::Ui::App>> makeApp() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        500,
        700,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);

    return {makeStrong<SdlApp>(window)};
}

} // namespace Embed
