#include <SDL.h>
#include <embed/ui.h>

namespace Embed {

struct SdlHost : public Ui::Host {
    SDL_Window *_window{};
    Math::Vec2i _lastMousePos{};

    SdlHost(Ui::Child root, SDL_Window *window)
        : Ui::Host(root), _window(window) {
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
        // Math::Recti and SDL_Rect should have the same layout so this is fine

        static_assert(sizeof(Math::Recti) == sizeof(SDL_Rect), "Rects must have the same layout");
        static_assert(offsetof(Math::Recti, x) == offsetof(SDL_Rect, x), "Rects must have the same layout");
        static_assert(offsetof(Math::Recti, y) == offsetof(SDL_Rect, y), "Rects must have the same layout");
        static_assert(offsetof(Math::Recti, width) == offsetof(SDL_Rect, w), "Rects must have the same layout");
        static_assert(offsetof(Math::Recti, height) == offsetof(SDL_Rect, h), "Rects must have the same layout");
        static_assert(Meta::Same<decltype(Math::Recti::x), decltype(SDL_Rect::x)>, "Rects must have the same layout");

        SDL_UpdateWindowSurfaceRects(
            _window,
            reinterpret_cast<const SDL_Rect *>(rects.buf()),
            rects.len());
    }

    void translate(SDL_Event const &sdlEvent) {
        switch (sdlEvent.type) {
        case SDL_WINDOWEVENT:
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

            Events::MouseEvent uiEvent{
                .type = Events::MouseEvent::MOVE,

                .pos = {sdlEvent.motion.x, sdlEvent.motion.y},
                .delta = {sdlEvent.motion.xrel, sdlEvent.motion.yrel},
            };

            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            uiEvent.buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            _lastMousePos = uiEvent.pos;

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
                .scroll = {sdlEvent.wheel.x, sdlEvent.wheel.y},
            };

            event(uiEvent);
            break;
        }

        case SDL_QUIT: {
            Events::ExitEvent uiEvent{OK};
            bubble(uiEvent);
            break;
        }

        default:
            break;
        }
    }

    void pump() override {
        SDL_Event e{};

        while (SDL_PollEvent(&e) != 0 && alive()) {
            translate(e);
        }
    }

    void wait(size_t ms) override {
        SDL_WaitEventTimeout(nullptr, ms);
    }
};

Result<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
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

    return {makeStrong<SdlHost>(root, window)};
}

} // namespace Embed
