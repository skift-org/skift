#include <libsystem/core/CString.h>
#include <libterminal/Renderer.h>
#include <libterminal/Terminal.h>

void terminal_renderer_initialize(TerminalRenderer *renderer)
{
    memset(renderer, 0, sizeof(TerminalRenderer));
}

void terminal_renderer_destroy(TerminalRenderer *renderer)
{
    if (renderer->destroy)
        renderer->destroy(renderer);

    free(renderer);
}
