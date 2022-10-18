#include <SDL.h>
#include <embed/ui.h>
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

    Gfx::Surface surface() override {
        SDL_Surface *s = SDL_GetWindowSurface(_window);

        return {
            Gfx::BGRA8888,
            {
                s->pixels,
                s->w,
                s->h,
                (size_t)s->pitch,
            },
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
            switch (sdlEvent.window.event) {

            case SDL_WINDOWEVENT_RESIZED:
                _shouldLayout = true;
                break;

            case SDL_WINDOWEVENT_EXPOSED:
                _dirty.pushBack(surface().bound());
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

            Math::Vec2i screenPos = {};
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
            _lastScreenMousePos = screenPos;

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
                .scrollLines = {sdlEvent.wheel.x, sdlEvent.wheel.y},
                .scrollPrecise = {sdlEvent.wheel.preciseX, sdlEvent.wheel.preciseY},
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

    void bubble(Events::Event &e) override {
        if (e.is<Ui::DragEvent>()) {
            auto &dragEvent = e.unwrap<Ui::DragEvent>();
            if (dragEvent.type == Ui::DragEvent::START) {
                SDL_CaptureMouse(SDL_TRUE);
            } else if (dragEvent.type == Ui::DragEvent::END) {
                SDL_CaptureMouse(SDL_FALSE);
            } else if (dragEvent.type == Ui::DragEvent::DRAG) {
                Math::Vec2i pos{};
                SDL_GetWindowPosition(_window, &pos.x, &pos.y);
                pos = pos + dragEvent.delta;
                SDL_SetWindowPosition(_window, pos.x, pos.y);
            }
        }

        Ui::Host::bubble(e);
    }
};

Result<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto size = root->size({700, 500}, Layout::Hint::MIN);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS | SDL_WINDOW_UTILITY);

    if (!window) {
        return Error{SDL_GetError()};
    }

    return {makeStrong<SdlHost>(root, window)};
}

} // namespace Embed
